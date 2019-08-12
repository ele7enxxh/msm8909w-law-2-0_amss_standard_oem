#ifndef A2_COMMON_H
#define A2_COMMON_H
/*!
  @file a2_common.h

  @brief
   A header specifying common types needed by different parts of the A2.

*/


/*===========================================================================

  Copyright (c) 2014 QUALCOMM Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/utils/api/a2/a2_common.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
03/16/15   ars     CR807123 support for windows aggregation
06/18/14   ars     MSM QMAP aggr feature for teth rmnet
01/15/14   vd      CR592276: Remove A2 extra dcinva instruction 
09/11/12   ars     QNX Flow control with QMAP
07/19/13   ars     QMAP FC Feature
07/10/13   vd      CR511894, Bug fixes to relocatable image
05/29/13   yjz     CR493599, BearerID api initial check in
05/08/13   vd      CR489850: Relocatable Memory
04/17/13   yjz     Add a2_timetick_t, CR462233
03/20/13   ars     CR465878: Added checksum offload feature
01/23/13   ars     QMAP changes for muxing
11/26/12   ag      Change for QTF build
07/31/12   ar      changed A2_TECHNOLOGY_DEFAULT enum value to 0
03/19/12   ar      added A2_PER_MUX_HDR_OPEN_CMD_RES_BIT0 macro
08/28/12   ars     Fixed the feature around cache invalidate 
04/11/12   bn      Added in support for DIME
03/19/12   ar      added A2_PER_MUX_HDR_OPEN_CMD_RES_BIT0 macro
02/29/12   kp      Added code for security algo ZUC.
01/10/12   bn      Added support to enable/disable PC using MUX command
10/06/11   ar      added a2 power collapse support for TDSCDMA
03/11/11   sm      Removed "barrier" call from a2_invalidate_dcache()
03/07/11   yuw     Modified macro name
03/05/11   yuw     Added A2_PER_CSFB_CTL_SIG_DTR_MASK
02/08/11   yuw     Supported tethered RmNet and DUN call on CSFB Fusion
09/06/10   sm      changed A2_TECHNOLOGY_CDMA_1X to A2_TECHNOLOGY_DO
08/25/10   enj     change T_QDSP6 to __qdsp6__
08/06/10   ar      increased the A2_MAX_SRC_IOVEC_COUNT_PER_DMA_CHAIN count to
                   200 to address long ip packets chains due to small TBs
08/06/10   ar      CR 248847 fix: removed A2_TECHNOLOGY_INVALID technology enum
07/08/10   bn      Increased the number of dsm per chain in iov to 120
06/16/10   ar      exported A2_INVALIDATE_CACHE macro and corresponding
                   a2_invalidate_dcache api
05/04/10   bn      Added in support for A2 DL muxing (FEATURE_A2_FUSION)
12/22/09   yuw     Check in on-target changes for framer and deframer
12/09/09   sm      Added A2_MAX_SRC_IOVEC_COUNT_PER_DMA_CHAIN
11/09/09   ar      extended a2_technology_e to support CDMA 1x and default mode
08/14/09   yuw     Supports SWI_Release_2009_07_24
06/17/09   ar      fixed comments for a2_integrity_algo_e enum
06/01/09   ar      added integrity and ciphering related enums and structures
05/27/09   sm      Added a2_crc_size_e and a2_endian_e
05/14/09   yuw     Added a2_status_e to indicate the return status of the A2 API functions
04/26/09   sm      Added a2_technology_e
04/22/09   sah     Add IP type to common header.
04/21/09   sm      Added some typedef/enums related to ciphering and iovec
03/20/09   sah     Add function pointer calback type.
01/20/09   sah     Add new hdr task
12/24/08   yg      Added new macro A2_BYTES_TO_WORDS_ROUNDUP.
08/08/08   ahe     Removed def to cell_id
07/11/08   sah     Initial Checkin
===========================================================================*/


/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include <comdef.h>

/*===========================================================================

                               MACROS

===========================================================================*/


#define A2_BYTES_TO_BITS(x) ((x)<<3)
#define A2_BITS_TO_BYTES(x) ((x)>>3)
#define A2_BITS_TO_BYTES_ROUNDUP(x) ((x+7)>>3)

/* @brief the maximum payload of the filltask in bytes */
#define A2_FILL_TASK_MAX_PAYLOAD 16

/*! @brief The maximum payload of the a2 hdr task in bits */
#define A2_HDR_TASK_MAX_PAYLOAD 32

#define A2_BITS_TO_WORDS(x) ((x)>>5)
#define A2_BYTES_TO_WORDS(x) ((x)>>2)
#define A2_BYTES_TO_WORDS_ROUNDUP(x) (((x)+3)>>2)

#define A2_WORDS_TO_BYTES(x) ((x)<<2)

/*! @brief Returns the number of bits of remainder when dividing by a
    word
 */
#define A2_BIT_WORD_REMAINDER(num_bits) ((num_bits)&0x1f)

/*!
  Maximum number of iovec (address+length) count per DMA chain
  TODO: need to optimize this number by either dropping long chains or by
  programming a2 dl per in parts. also this macro is common for multiple modules
  such as UL SEC(for hspa apis) and PER. Not all modules need high IOVEC COUNT
  */
#define A2_MAX_SRC_IOVEC_COUNT_PER_DMA_CHAIN 200

/*set the unique_id for dsm ptr*/
#define A2_COMMON_SET_DSM_PKT_UNIQUE_ID(dsm_packet_ptr, unique_id) \
           (dsm_packet_ptr)->kind = unique_id
/*get the unique_id for dsm ptr*/
#define A2_COMMON_GET_DSM_PKT_UNIQUE_ID(dsm_packet_ptr) \
           (dsm_packet_ptr)->kind
/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/


/* @brief maximum value of cipher key index */
#define A2_CIPHER_MAX_KEY_IDX   15

/* @brief maximum number of cipher key indexes (valid range 0..15) */
#define A2_CIPHER_MAX_NUM_KEY_IDX   16

/*! cache line size in bytes  */
#define A2_CACHELINE_SIZE 32


/*! @brief Aligns the address to the previous cache aligned address
  */
#define A2_CACHE_ALIGN(addr) \
  ((void*)(((uint32)(addr))& (~((A2_CACHELINE_SIZE)-1))))

/*!
  @brief
  The system and subframe number.

  @note  This may be a common structure and not defined by the a2.
  */
//typedef uint32 lte_sfn_t;

typedef enum
{
  /* A2 internal memory where Ciphering Keys, filters etc needs to be stored */
  A2_MEM_INTERNAL,
  /*! Encoder RAM */
  A2_MEM_ERAM,
  /*! Decoder RAM */
  A2_MEM_DOB,
  /*! BAM (USB end points) */
  A2_MEM_BAM,
  /*! external memory (where DSM items etc resides) */
  A2_MEM_EXTERNAL
} a2_mem_type_e;

typedef enum
{
  A2_IPV4,
  A2_IPV6,
  A2_IP_NUM_TYPES
} a2_ip_type_e;
/*!
  @brief enumeration to indicate which technology is using A2 HW
  */
typedef enum
{
  A2_TECHNOLOGY_DEFAULT,  /*!< DEFAULT tech (NONE) */
  A2_TECHNOLOGY_HSPA,     /*!< HSPA tech */
  A2_TECHNOLOGY_LTE,      /*!< LTE tech */
  A2_TECHNOLOGY_DO,       /*!< CDMA Do tech */
  A2_TECHNOLOGY_TDSCDMA,  /*!< TDSCDMA tech */
} a2_technology_e;

/*!
  @brief
  enumeration to indicate the return status of the A2 API functions
  Generic outcomes of A2 operations
*/
typedef enum
{
  A2_STATUS_SUCCESS = 0, /*!<   Operation successful */
  A2_STATUS_FAILURE      /*!< Operation failed due to err */
} a2_status_e;

/*!
  @brief
  enumeration for endianess of memory.
  Value of enumeration matches with the endianess setting of A2 SWI.
*/
typedef enum
{
  A2_ENDIAN_LITTLE = 0, /*!< Little Endian */
  A2_ENDIAN_BIG         /*!< Big Endian */
} a2_endian_e;

/*! @brief Which direction the ciphering parameters should be set for.
    @note These enums are same as HW defined definations. See a2_init_verify().
    @todo need to see how we can make sure that enum doesnt always result in
     4 bytes allocation on windows and above target. May be used PACKED. Can we
     avoid PACKED?
*/
typedef enum
{
  A2_CIPHER_DIRECTION_UL = 0,
  A2_CIPHER_DIRECTION_DL = 1,
} a2_cipher_direction_t;

/*! @brief Which direction the integrity parameters should be set for.
    @note These enums are same as HW defined definitions. See a2_init_verify().
    @todo need to see how we can make sure that enum doesnt always result in
    4 bytes allocation on windows and above target. May be used PACKED. Can we
    avoid PACKED?
*/
typedef enum
{
  A2_INTEGRITY_DIRECTION_UL = 0,
  A2_INTEGRITY_DIRECTION_DL = 1,
} a2_integrity_direction_t;

/*! @brief Ciphering Algorithm
    @note These enums are same as HW defined definations. See a2_init_verify().
    @todo need to see how we can make sure that enum doesnt always result in
     4 bytes allocation on windows and above target. May be used PACKED. Can we
     avoid PACKED?
*/
typedef enum
{
  /*! UMTS KASUMI; equivalent to uea1 */
  A2_CIPHER_ALGO_KASUMI       = 0,
  /*! UMTS SNOW-3G; equivalent to uea2 */
  A2_CIPHER_ALGO_UMTS_SNOW3G  = 1,
  /*! LTE SNOW-3G; equivalent to eea1 */
  A2_CIPHER_ALGO_LTE_SNOW3G   = 2,
  /*! LTE AES; equivalent to eea2 */
  A2_CIPHER_ALGO_AES          = 3,
  /*!< LTE ZUC; equivalent to eea3 or spare5 */
  A2_CIPHER_ALGO_ZUC         = 4,
  A2_CIPHER_ALGO_NONE         = 0x7, /*!< No Ciphering; equivalent to
                                          uea0/eea0 */
} a2_cipher_algo_e;

/*! @brief Integrity Algorithm
    @note These enums are same as HW defined definations. See a2_init_verify().
    @todo need to see how we can make sure that enum doesnt always result in
    4 bytes allocation on windows and above target. May be used PACKED. Can we
    avoid PACKED?
*/
typedef enum
{
  A2_INTEGRITY_ALGO_KASUMI      = 0,   /*!< UMTS KASUMI; equivalent to uea1 */
  A2_INTEGRITY_ALGO_LTE_SNOW3G  = 1,   /*!< LTE SNOW-3G; equivalent to eea1 */
  A2_INTEGRITY_ALGO_AES         = 2,   /*!< LTE AES; equivalent to eia2 */
  A2_INTEGRITY_ALGO_ZUC         = 3,   /*!< LTE AES; equivalent to eia3-v11xy or spare5 */
  A2_INTEGRITY_ALGO_NONE        = 0x7, /*!< No integrity; equivalent to
                                            uia0/eia0 */
} a2_integrity_algo_e;

/*!
   @brief
   structure to hold ciphering information
   NOTE: If cipher algo is set to A2_CIPHER_ALGO_NONE then remaining fields are
   don't care.
*/
typedef struct
{
  /*! The type of ciphering algorithm to use. If set to A2_CIPHER_ALGO_NONE
      then data will not be ciphered and remaining fields will be ignored */
  a2_cipher_algo_e       algo;
  /*! Ciphering key index. Valid values: 0..15 */
  uint8                  key_index;
  /*! radio_bearer_id minus 1. Valid values: 0..31 */
  uint8                  bearer_id;
  /*! The count-c to be used for ciphering, count-c is same as count for LTE */
  uint32                 count_c;
} a2_cipher_info_s;

/*!
   @brief
   structure to hold integrity information
   NOTE: If integrity algo is set to A2_INTEGRITY_ALGO_NONE then remaining fields
   are don't care.
*/
typedef struct
{
  /*! Type of integrity algorithm to use. If set to A2_INTEGRITY_ALGO_NONE then
   * integrity will not be computed and remaining fields will be ignored */
  a2_integrity_algo_e       algo;
  /*! integrity key index. Valid values: 0..15 */
  uint8                     key_index;
  /*! radio_bearer_id minus 1. Valid values: 0..31 */
  uint8                     bearer_id;
  /*! UL or DL direction */
  a2_integrity_direction_t  dir;
  /*! The count to be used for integrity */
  uint32                    count;
} a2_integrity_info_s;

/*!
  @brief
  structure to hold iovec (address and length)
*/
typedef struct
{
  uint32 addr;            /*!< address */
  uint32 length_in_bytes; /*!< The length in bytes */
} a2_iovec_t;

/*!
  @brief
  time type to replace timetick_type
*/
typedef uint32 a2_timetick_t;

/* @brief a void parameter/return value function pointer type.

   */
typedef void (*a2_vv_func_ptr_t)( void);

/*!
  Enum identifying endpoint id.
  */
typedef enum
{
  A2_ENDPOINT_0 = 0,      /*!<  USB endpoint 0  */
  A2_ENDPOINT_1,          /*!<  USB endpoint 1  */
  A2_ENDPOINT_2,          /*!<  USB endpoint 2  */
  A2_ENDPOINT_EXT_MEM,    /*!<  USB endpoint external memory  */
  A2_MAX_NUM_ENDPOINT     /*!<  number of endpoints  */
} a2_endpoint_id_e;

/*!
  Enum identifying Protocol Accelerator Selection for each endpoint.
  */
typedef enum
{
  A2_ENDPOINT_BYPASS = 0,      /*!<  bypass  */
  A2_ENDPOINT_PPP              /*!<  PPP     */
} a2_endpoint_mode_e;

/*! @brief defined the magic number for the MUX header */
#define A2_PER_MUX_HDR_MAGIC            0x33FC

/*! @brief defined the data command for MUX header */
#define A2_PER_MUX_HDR_DATA_CMD         0x0

/*! @brief defined the open command for MUX header with PC enable*/
#define A2_PER_MUX_HDR_OPEN_CMD         0x1

/*! @brief defined the close command for MUX header */
#define A2_PER_MUX_HDR_CLOSE_CMD        0x2

/*! @brief defined the type of modem status in the MUX header*/
#define A2_PER_MUX_HDR_MODEM_STATUS_CMD        0x3

/*! @brief defined the open command for MUX header with PC disable */
#define A2_PER_MUX_HDR_OPEN_PC_DISABLE_CMD   0x4

/*! @brief defined the flow control command */
#define A2_PER_MUX_HDR_QMAP_FLOW_CTRL_CMD   0x5

/*! @brief bit value for the reserved field (BIT 0) for OPEN command, indicates
  whether a2 driver expects an ACK in response to the NOTIFY PC request sent to
  Apps proc */
#define A2_PER_MUX_HDR_OPEN_CMD_RES_BIT0_MASK     0x1

/*! @brief bit mask for the reserved field (BIT 1) for OPEN command, indicates
  whether A2/BAM DMUX support QMAP over BAM DMUX */
#define A2_PER_MUX_HDR_OPEN_CMD_RES_BIT1_MASK     0x2

/*! @brief bit mask for the reserved field (BIT 2) for OPEN command, indicates
  the default DL descriptor size used by BAM DMUX
  0 - 2K, 1 - 4K */
#define A2_PER_MUX_HDR_OPEN_CMD_RES_BIT2_MASK     0x4
/*! @brief bit mask for the reserved field (BIT 3) for OPEN command, indicates
  if target is Linux or Windows. 0 - Linux, 1 - Windows 
*/
#define A2_PER_MUX_HDR_OPEN_CMD_RES_BIT3_MASK     0x8

/*! @brief bit mask for the reserved field (BIT 4 and 5) for OPEN command, 
  indicates the max DL descriptor count in BAM DMUX */
#define A2_PER_MUX_HDR_OPEN_CMD_RES_BIT4_5_MASK   0x30

/*! @brief bit mask for the reserved field (BIT 6 and 7) for OPEN command, 
  indicates the UL/DL aggregation limits */
#define A2_PER_MUX_HDR_OPEN_CMD_RES_BIT6_7_MASK   0xC0

/*! @brief defined bit mask for modem status */
#define A2_PER_CSFB_MODEM_CTL_SIG_MASK       0x0F

/*! @brief defined bit mask for DSR */
#define A2_PER_CSFB_CTL_SIG_DSR_MASK       0x01

/*! @brief defined bit mask for DTR */
#define A2_PER_CSFB_CTL_SIG_DTR_MASK       0x01

/*! @brief defined bit mask for RTR */
#define A2_PER_CSFB_CTL_SIG_RTR_MASK       0x02

/*! @brief defined bit mask for RI */
#define A2_PER_CSFB_CTL_SIG_RI_MASK       0x04

/*! @brief defined bit mask for DCD */
#define A2_PER_CSFB_CTL_SIG_DCD_MASK       0x08

/*! @brief defined macro for the new port  */
#define A2_VIRTUAL_PORT       A2_ENDPOINT_EXT_MEM

/*! @brief defined error for function return  */
#define A2_ERR       -1

/* !@brief: Size of QMAP control header (in bytes) which includes the QMAP header*/
#define A2_COMMON_QMAP_CONTROL_HDR_SIZE 12

#define A2_COMMON_QMAP_CONTROL_MAX_PAYLOAD_SIZE 20

/*! @brief defined enumeration of UL and DL aggregation limits for MSM */
typedef enum
{
  A2_PER_AGGR_2K =0,
  A2_PER_AGGR_4K,
  A2_PER_AGGR_8K,
  A2_PER_AGGR_16K,
  A2_PER_AGGR_MAX /*! Boundary check only */
} a2_per_aggr_e;

/*! @brief defined enumeration for BAM DMUX descriptor size for MSM */
typedef enum
{
  A2_PER_BAM_DESC_SIZE_2K =0,
  A2_PER_BAM_DESC_SIZE_4K,
  A2_PER_BAM_DESC_SIZE_MAX /*! Boundary check only */
} a2_per_bam_desc_size_e;

/*! @brief defined enumeration for BAM DMUX descriptor count for MSM */
typedef enum
{
  A2_PER_BAM_DESC_CNT_32 =0,
  A2_PER_BAM_DESC_CNT_64,
  A2_PER_BAM_DESC_CNT_128,
  A2_PER_BAM_DESC_CNT_256,
  A2_PER_BAM_DESC_CNT /*! Boundary check only */
} a2_per_bam_desc_count_e;

/*! @brief defined enumeration for BAM DMUX HLOS type MSM */
typedef enum
{
  A2_PER_BAM_HLOS_LINUX =0,
  A2_PER_BAM_HLOS_WINDOWS
}a2_per_bam_hlos_e;

/*! @brief defined enumeration of muxed port status */
typedef enum
{
  /*! no muxing on this port */
  A2_PER_MUX_NONE,
  /*! mux port is pending on open/close*/
  A2_PER_MUX_PENDING,
  /*! mux port is opened or ready to send/receive */
  A2_PER_MUX_OPEN,
  /*! mux port is closed */
  A2_PER_MUX_CLOSE
} a2_per_mux_port_status_e;

/*! @brief defined enumeration of type of mux packets */
typedef enum
{
  /*! command packet */
  A2_QMAP_HDR_DATA = 0,
  /*! Data packet */
  A2_QMAP_HDR_CTRL
} a2_qmap_pkt_type_e;

/*! @brief A2 PER muxing header structure */
typedef struct
{
  /*! magic number. Always needs to be set to
  A2_PER_MUX_HDR_MAGIC */
  uint16  magic_num;
  /*! For DUN signal transfer on control path 
      or shall be set to 0 by sender for data path */
  uint8   reserved_or_signal;
  /*! command type, will be set to A2_PER_MUX_HDR_CMD */
  uint8   command;
  /*! padding bytes to make packet length word aligned..0..3 */
  uint8   padding_bytes;
  /*! logical channel id; pipe/port id */
  uint8   lc_id;
  /*! IP packet length (may or maynot include length...TBD) */
  uint16  pkt_length;
} a2_per_mux_header_s;

/*! QMAP mux header */
typedef struct
{
  /*! packet length with padding  */
  uint32 packet_padding_len : 16;                           /* W0 [0:15] */
  /*! QMAP mux id from 1 to 18 */
  uint32 mux_id : 8;                                        /* W0 [16:23] */
   /*! Padding length to make the data block to word alignment */
  uint32 padding_len : 6;                                   /* W0 [24:29] */
    /*! reserved bit. Should be set to zero*/
  uint32 res_bit : 1;                                       /* W0 [30:30] */
  /*! indication bit : 0 = Data, 1 = Control */
  uint32 hdr_type : 1;                                      /* W0 [31:31] */
} a2_per_qmap_header_s;

/*! QMAP V2 mux header */

typedef struct
{
  /*! packet length with padding  */
  uint32 packet_padding_len : 16;                           /* W0 [0:15] */
  /*! QMAP mux id from 1 to 18 */
  uint32 mux_id : 8;                                        /* W0 [16:23] */
   /*! Padding length to make the data block to word alignment */
  uint32 padding_len : 6;                                   /* W0 [24:29] */
    /*! reserved bit. Should be set to zero*/
  uint32 res_bit : 1;                                       /* W0 [30:30] */
  /*! indication bit : 0 = Data, 1 = Control */
  uint32 hdr_type : 1;                                      /* W0 [31:31] */

  /*! Checksum insert offset */
  uint32 cksum_insert_offset : 14;                          /* W1 [0:13] */
  /*! UDP IPv4 Indicator*/
  uint32 udp_ipv4_ind : 1;                                  /* W1 [14:14] */
  /*! Checksum enable */
  uint32 cksum_enable : 1;                                  /* W1 [15:15] */
  /*! Checksum start offset  */
  uint32 cksum_start_offset : 16;                           /* W1 [16:31] */
} a2_per_qmap_v2_header_s;


/* !@brief: Enum for QMAP control command type */
typedef enum
{
  /* Request, i.e. sender is sending a QMAP control command to the receiver*/
  A2_COMMON_QMAP_CTRL_CMD_REQ=0,
  /* Ack, i.e. receiver is acknowledging that it received a QMAP
     control command and that, it successfully processed the command */
  A2_COMMON_QMAP_CTRL_CMD_ACK,
  /* Unsupported command, i.e. receiver doesn’t support this
     QMAP control command */
  A2_COMMON_QMAP_CTRL_CMD_UNSUPP,
  /* Invalid command, i.e. receiver encountered an error while
     processing the QMAP control command, probably because QMAP
     control command is malformed */
  A2_COMMON_QMAP_CTRL_CMD_INV,
  /* Boundary check */
  A2_COMMON_QMAP_CTRL_CMD_MAX
}a2_common_qmap_ctrl_cmd_type_e;

/* !@brief: Enum for QMAP control command name */
typedef enum
{
  A2_COMMON_QMAP_MIN_CMD=0,
  /* Enable Flow Control. i.e Disable Flow */
  A2_COMMON_QMAP_FLOW_CONTROL_EN_CMD,
  /* Disable Flow Control. i.e Enable Flow */
  A2_COMMON_QMAP_FLOW_CONTROL_DIS_CMD,
  A2_COMMON_QMAP_MAX_CMD
}a2_common_qmap_ctrl_cmd_name_e;

/*! @brief defined enumeration of mux type in A2 */
typedef enum
{
  A2_MUX_NONE = 0,
  A2_MUX_FUSION_LEGACY,
  A2_MUX_TLP_LEGACY,
  A2_MUX_QCNCM,
  A2_MUX_QMAP,
  A2_MUX_QMAP_V2,/* For A2 SW internal use only */
  A2_MUX_MBIM,
  A2_MUX_BAM_DMUX_QMAP,
  A2_MUX_MAX
}a2_mux_type_e;

typedef struct
{
  /*! Reserve bits */
  uint32 reserve_1 : 16;                                    /* W1 [0:15] */
  /*! Command type 0 = Req, 1 = ACK, 2 = UNSUPP, 3 = INV */
  uint32 cmd_type : 2;                                      /* W1 [16:17] */
  /*! Reserve bits */
  uint32 reserve_2 : 6;                                     /* W1 [18:23] */
  /*! Command name */
  uint32 cmd_name : 8;                                      /* W1 [24:31] */
  /*! Transaction ID or a timestamp */
  uint32 transaction_id : 32;                                /* W2 [0:31] */ 
}a2_common_qmap_cmd_info_s;

/*! QMAP control command header */
typedef struct
{
  a2_per_qmap_header_s       hdr;
  a2_common_qmap_cmd_info_s  cmd_info;
} a2_common_qmap_ctrl_cmd_s;

/* Macro input is based on enum a2_per_bam_desc_cnt_e */
#define A2_PER_CONVERT_ENUM_TO_BAM_DESC_CNT(desc_cnt_enum) \
  ((32) << (desc_cnt_enum))

/* Macro input is based on enum a2_per_aggr_e */
#define A2_PER_CONVERT_ENUM_TO_AGGR_BYTES(aggr_enum) \
  ((2048) << (aggr_enum))

  /* Macro input is based on enum a2_per_bam_desc_size_e */
#define A2_PER_CONVERT_ENUM_TO_BAM_DESC_SIZE(desc_size_enum) \
  ((2048) << (desc_size_enum))

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

#ifndef T_WINNT

/*===========================================================================

  FUNCTION:  a2_invalidate_dcache

===========================================================================*/
/*!
    @brief
    Invalidates data cache.

    @detail
    Invalidates mem_size bytes of memory from starting
    address physical_addr.

    @note:
      A) If number of bytes are not divisible by cache line size
         (for example 32 bytes), this function will invalidate alittle
         bit more (below is number of bytes to be invalidated)
         ceil(mem_size/cache_line_size)*cache_line_size. To be
         safe, mem_size should be a multiple of cache line size :-)

      B) This function assumps physical_addr is cache aligned. If
         physical_addr is not cache aligned, more data will be invalidated
         at the beginning and at the end (as long as physical_addr will
         be in a cache line, that line will be invalidated)

      c) If memory is both L1 and L2 cacheable, then two invalidates
         are needed. This is fixed in QDSP6 version>3. Currently
         L1 cacheable memory allocation (write through/write back),
         will be always L2 cacheable using CS (will be fixed later)

      d) Volatile has been added to each command to make sure
         that after calling this function, memory is correct and
         prevent any issue by compiler optimization.
         After moving to source directory, modem_fw_util.h has
         all these defined as static INLINE functions, and this
         function can be modified to follow WiMAX methodology.

      e) mem_size has to be non-zero

    @todo Verify barrier is necessary.

    @return
    memory will be invalidated
*/
/*=========================================================================*/
INLINE void a2_invalidate_dcache(uint32 physical_addr, uint32 mem_size)
{

  uint32 cur_cache_line;
  uint32 last_cache_line;

  /* The start of the first cache line */
  cur_cache_line = (uint32)A2_CACHE_ALIGN( physical_addr );

  /* The start address of the last cache line.  mem_size - 1 is used
     because bytes 0--(n-1) need to be flushed which translates to n bytes.
     */
  last_cache_line = (uint32)A2_CACHE_ALIGN( physical_addr + mem_size - 1 );

  /* Flush one cacheline at a time */
  for (;
      cur_cache_line <= last_cache_line;
      cur_cache_line += A2_CACHELINE_SIZE)
  {
    /* use inline assembly code to do invalidate a cache line
       dcinva(Rs= (uint32 *) physical_addr); */
    asm volatile ("dcinva(%0)" : : "r" ((uint32 *)( cur_cache_line )));
  }
} /* a2_invalidate_dcache */

/*!
   @brief
    Macro to invalidate the cache.  This will be substituted with the OS
    mechanism at a later time

   @todo need to make sure whether simply doing invalidate is fine for
   destination address becasue there is a possibiliy that someone might have
   modified at the beginning and end of the destination. May be flush just
   the beginning and end cache lines for a given destination.
*/
#define A2_CACHE_INVALIDATE(start_addr, length_to_flush)  \
  a2_invalidate_dcache(start_addr, length_to_flush)

#else
#error code not present
#endif /* T_WINNT */

#endif /* A2_COMMON_H */

