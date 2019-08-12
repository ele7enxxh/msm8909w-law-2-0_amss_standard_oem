#ifndef __SDCC_BAM_H
#define __SDCC_BAM_H
/**********************************************************************
 * sdcc_bam.h
 *
 * This file provides definition for BAM supporting function
 *
 * Copyright (c) 2011 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 *
 **********************************************************************/
/*======================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/sdcc/src/sdcc_bam.h#1 $
  $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when         who     what, where, why
----------   ---     -----------------------------------------------
2011-12-14   rh      Initial version
==================================================================*/

/**********************************************************/
/*                    INCLUDES                            */
/**********************************************************/
#include "sdcc_priv.h"

/* * DML Memory-mapped registers */
#define DML_CONFIG(_b)                          ((_b) + 0x00)
#define DML_STATUS(_b)                          ((_b) + 0x04)
#define DML_SW_RESET(_b)                        ((_b) + 0x08)
#define DML_PRODUCER_START(_b)                  ((_b) + 0x0C)
#define DML_CONSUMER_START(_b)                  ((_b) + 0x10)
#define DML_PRODUCER_PIPE_LOGICAL_SIZE(_b)      ((_b) + 0x14)
#define DML_CONSUMER_PIPE_LOGICAL_SIZE(_b)      ((_b) + 0x18)
#define DML_PIPE_ID(_b)                         ((_b) + 0x1C)
#define DML_PRODUCER_BAM_BLOCK_SIZE(_b)         ((_b) + 0x24)
#define DML_PRODUCER_BAM_TRANS_SIZE(_b)         ((_b) + 0x28)

/* * BAM Memory-mapped registers */
#define BAM_CTRL(_b)                            ((_b) + 0xF80 + 0x00)
#define BAM_REVISION(_b)                        ((_b) + 0xF80 + 0x04)
#define BAM_NUM_PIPES(_b)                       ((_b) + 0xF80 + 0x3C)
#define BAM_DESC_CNT_TRSHLD(_b)                 ((_b) + 0xF80 + 0x08)
#define BAM_IRQ_SRCS(_b)                        ((_b) + 0xF80 + 0x0C)
#define BAM_IRQ_SRCS_MSK(_b)                    ((_b) + 0xF80 + 0x10)
#define BAM_IRQ_SRCS_UNMASKED(_b)               ((_b) + 0xF80 + 0x30)
#define BAM_IRQ_STTS(_b)                        ((_b) + 0xF80 + 0x14)
#define BAM_IRQ_CLR(_b)                         ((_b) + 0xF80 + 0x18)
#define BAM_IRQ_EN(_b)                          ((_b) + 0xF80 + 0x1C)
#define BAM_IRQ_SIC_SEL(_b)                     ((_b) + 0xF80 + 0x20)
#define BAM_AHB_MASTER_ERR_CTRLS(_b)            ((_b) + 0xF80 + 0x24)
#define BAM_AHB_MASTER_ERR_ADDR(_b)             ((_b) + 0xF80 + 0x28)
#define BAM_AHB_MASTER_ERR_DATAS(_b)            ((_b) + 0xF80 + 0x2C)
#define BAM_IRQ_DEST_ADDR(_b)                   ((_b) + 0xF80 + 0x34)
#define BAM_PERIPH_IRQ_DEST_ADDR(_b)            ((_b) + 0xF80 + 0x38)
#define BAM_CNFG_BITS(_b)                       ((_b) + 0xF80 + 0x7C)

#define BAM_P_CTRLn(_b, n)                      ((_b) + n*128 + 0x00)
#define BAM_P_RSTn(_b, n)                       ((_b) + n*128 + 0x04)
#define BAM_P_HALTn(_b, n)                      ((_b) + n*128 + 0x08)
#define BAM_P_IRQ_STTSn(_b, n)                  ((_b) + n*128 + 0x10)
#define BAM_P_IRQ_CLRn(_b, n)                   ((_b) + n*128 + 0x14)
#define BAM_P_IRQ_ENn(_b, n)                    ((_b) + n*128 + 0x18)
#define BAM_P_TIMERn(_b, n)                     ((_b) + n*128 + 0x1C)
#define BAM_P_TIMER_CTRLn(_b, n)                ((_b) + n*128 + 0x20)

#define BAM_P_EVNT_DEST_ADDRn(_b, n)            ((_b) + n*64 + 0x102C)
#define BAM_P_EVNT_REGn(_b, n)                  ((_b) + n*64 + 0x1018)
#define BAM_P_SW_OFSTSn(_b, n)                  ((_b) + n*64 + 0x1000)
#define BAM_P_DATA_FIFO_ADDRn(_b, n)            ((_b) + n*64 + 0x1024)
#define BAM_P_DESC_FIFO_ADDRn(_b, n)            ((_b) + n*64 + 0x101C)
#define BAM_P_EVNT_GEN_TRSHLDn(_b, n)           ((_b) + n*64 + 0x1028)
#define BAM_P_FIFO_SIZESn(_b, n)                ((_b) + n*64 + 0x1020)
#define BAM_P_IRQ_DEST_ADDRn(_b, n)             ((_b) + n*64 + 0x103C)

#define BAM_DESC_EOT_BIT            (1 << 30)
#define BAM_DESC_INT_BIT            (1 << 31)
#define BAM_DESC_LEN_BMSK           (0x0000ffff)
#define BAM_MAX_XFER_SIZE           (0x0000ffff)

// p = producer, c = consumer
#define DML_PIPE_ID_HFNC(_p, c)     ((_p & 0x0f) | ((c & 0x0f) << 16))

#define DML_CONFIG_CRCI(_p, c)      ((_p & 0x03) | ((c & 0x03) << 2))
#define DML_PRODUCER_TRANS_END_EN   (1 << 4)
#define DML_BYPASS                  (1 << 16)

#define BAM_CTRL_RESET              (1 << 0)
#define BAM_CTRL_EN                 (1 << 1)

#define BAM_CHICKEN_BIT             (0x7FFF004)
#define BAM_DESC_TRSHLD_DEFAULT     (0x400)

#define BAM_P_RESET                 (1 << 0)

#define BAM_P_FIFO_SIZE_DEFAULT     (0x80)
#define BAM_P_EVNT_GEN_TRSHLD_D     (0x200)

#define BAM_P_CTRL_PEN              (1 << 1)
#define BAM_P_WRITE                 (1 << 3)
#define BAM_P_SYSTEM_MODE           (1 << 5)
#define BAM_P_PRODUCER_START        (BAM_P_CTRL_PEN | BAM_P_WRITE | BAM_P_SYSTEM_MODE)
#define BAM_P_CONSUMER_START        (BAM_P_CTRL_PEN | BAM_P_SYSTEM_MODE)

#define BAM_P_IRQ_STTS_TRANSFER_END (1 << 5)

#define HWIO_REG_IN(reg)            (in_dword(reg))
#define HWIO_REG_OUT(reg, v)        (out_dword(reg, v))

typedef struct _sdcc_bam_priv_data
{
   uint32 bam_reg_base;       // Base register address for BAM
   uint32 dml_reg_base;       // Base register address for DML
   uint32 *desc_buffer;       // Pointer to a buffer that hold the descriptor
   uint32 desc_buf_phy;       // Physical address of the descriptor buffer 
   uint32 last_op_addr;       // Address of the last operation
   uint32 last_op_len;        // Length of the last operation
} bam_pdata_t;


#endif /* __SDCC_PAL_TOOL_H */

