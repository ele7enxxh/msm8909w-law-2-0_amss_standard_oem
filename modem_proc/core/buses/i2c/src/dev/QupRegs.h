#ifndef __QUPREGS_H_
#define __QUPREGS_H_

typedef enum I2C_MASTER_STATUS_CANCEL_FSM_STATE_Value {
   I2C_MASTER_STATUS_CANCEL_FSM_STATE_IDLE_STATE    = 0,
   I2C_MASTER_STATUS_CANCEL_FSM_STATE_CANCEL_PENDING_STATE   = 1,
   I2C_MASTER_STATUS_CANCEL_FSM_STATE_PAUSE_STATE   = 2,
   I2C_MASTER_STATUS_CANCEL_FSM_STATE_PAUSE_WAIT_STATE   = 3,
   I2C_MASTER_STATUS_CANCEL_FSM_STATE_SEND_STOP_STATE   = 4,
   I2C_MASTER_STATUS_CANCEL_FSM_STATE_FLUSH_STATE   = 5,
   I2C_MASTER_STATUS_CANCEL_FSM_STATE_SEND_IRQ_STATE   = 6,
} I2C_MASTER_STATUS_CANCEL_FSM_STATE_Value;

typedef enum I2C_MASTER_STATUS_INPUT_FSM_STATE_Value {
   I2C_MASTER_STATUS_INPUT_FSM_STATE_V1_V2_RESET_STATE   = 0,
   I2C_MASTER_STATUS_INPUT_FSM_STATE_V1_V2_IDLE_STATE   = 1,
   I2C_MASTER_STATUS_INPUT_FSM_STATE_V1_V2_READ_LAST_BYTE_STATE   = 2,
   I2C_MASTER_STATUS_INPUT_FSM_STATE_V1_V2_MI_REC_STATE   = 3,
   I2C_MASTER_STATUS_INPUT_FSM_STATE_V1_V2_DEC_STATE   = 4,
   I2C_MASTER_STATUS_INPUT_FSM_STATE_V1_V2_STORE_STATE   = 5,
   I2C_MASTER_STATUS_INPUT_FSM_STATE_V2_WR_TAG_STATE   = 6,
   I2C_MASTER_STATUS_INPUT_FSM_STATE_V2_WAIT_TAG_STATE   = 7,
} I2C_MASTER_STATUS_INPUT_FSM_STATE_Value;

typedef enum I2C_MASTER_STATUS_OUTPUT_FSM_STATE_Value {
   I2C_MASTER_STATUS_OUTPUT_FSM_STATE_V1_V2_RESET_STATE   = 0,
   I2C_MASTER_STATUS_OUTPUT_FSM_STATE_V1_V2_IDLE_STATE   = 1,
   I2C_MASTER_STATUS_OUTPUT_FSM_STATE_V1_V2_DECODE_STATE   = 2,
   I2C_MASTER_STATUS_OUTPUT_FSM_STATE_V1_V2_SEND_STATE   = 3,
   I2C_MASTER_STATUS_OUTPUT_FSM_STATE_V1_V2_MI_REC_STATE   = 4,
   I2C_MASTER_STATUS_OUTPUT_FSM_STATE_V1_V2_NOP_STATE   = 5,
   I2C_MASTER_STATUS_OUTPUT_FSM_STATE_V1_V2_INVALID_STATE   = 6,
   I2C_MASTER_STATUS_OUTPUT_FSM_STATE_V1_PEEK_STATE   = 7,
   I2C_MASTER_STATUS_OUTPUT_FSM_STATE_V1_SEND_R_STATE   = 8,
   I2C_MASTER_STATUS_OUTPUT_FSM_STATE_V2_GET_BYTE_STATE   = 11,
   I2C_MASTER_STATUS_OUTPUT_FSM_STATE_V2_GET_WRITE_BYTE_STATE   = 12,
   I2C_MASTER_STATUS_OUTPUT_FSM_STATE_V2_SPECIALITY_STATE   = 13,
   I2C_MASTER_STATUS_OUTPUT_FSM_STATE_V2_WAIT_FLUSH_STOP_STATE   = 14,
   I2C_MASTER_STATUS_OUTPUT_FSM_STATE_V2_STOP_STATE   = 15,
} I2C_MASTER_STATUS_OUTPUT_FSM_STATE_Value;

typedef enum I2C_MASTER_STATUS_CLK_STATE_Value {
   I2C_MASTER_STATUS_CLK_STATE_RESET_BUSIDLE_STATE   = 0,
   I2C_MASTER_STATUS_CLK_STATE_NOT_MASTER_STATE     = 1,
   I2C_MASTER_STATUS_CLK_STATE_HIGH_STATE           = 2,
   I2C_MASTER_STATUS_CLK_STATE_LOW_STATE            = 3,
   I2C_MASTER_STATUS_CLK_STATE_HIGH_WAIT_STATE      = 4,
   I2C_MASTER_STATUS_CLK_STATE_FORCED_LOW_STATE     = 5,
   I2C_MASTER_STATUS_CLK_STATE_HS_ADDR_LOW_STATE    = 6,
   I2C_MASTER_STATUS_CLK_STATE_DOUBLE_BUFFER_WAIT_STATE   = 7,
} I2C_MASTER_STATUS_CLK_STATE_Value;

typedef enum I2C_MASTER_STATUS_DATA_STATE_Value {
   I2C_MASTER_STATUS_DATA_STATE_RESET_WAIT_STATE    = 0,
   I2C_MASTER_STATUS_DATA_STATE_TX_ADDR_STATE       = 1,
   I2C_MASTER_STATUS_DATA_STATE_TX_DATA_STATE       = 2,
   I2C_MASTER_STATUS_DATA_STATE_TX_HS_ADDR_STATE    = 3,
   I2C_MASTER_STATUS_DATA_STATE_TX_10_BIT_ADDR_STATE   = 4,
   I2C_MASTER_STATUS_DATA_STATE_TX_2ND_BYTE_STATE   = 5,
   I2C_MASTER_STATUS_DATA_STATE_RX_DATA_STATE       = 6,
   I2C_MASTER_STATUS_DATA_STATE_RX_NACK_HOLD_STATE   = 7,
} I2C_MASTER_STATUS_DATA_STATE_Value;

typedef volatile struct QupRegs {
   uint32 QUP_CONFIG;                                       /* OFFSET 0x0 */
   uint32 QUP_STATE;                                        /* OFFSET 0x4 */
   uint32 QUP_IO_MODES;                                     /* OFFSET 0x8 */
   uint32 QUP_SW_RESET;                                     /* OFFSET 0xc */
   const uint32 _FILL0[1];                                  /* OFFSET 0x10 */
   uint32 QUP_TRANSFER_CANCEL;                              /* OFFSET 0x14 */
   uint32 QUP_OPERATIONAL;                                  /* OFFSET 0x18 */
   uint32 QUP_ERROR_FLAGS;                                  /* OFFSET 0x1c */
   uint32 QUP_ERROR_FLAGS_EN;                               /* OFFSET 0x20 */
   uint32 QUP_TEST_CTRL;                                    /* OFFSET 0x24 */
   uint32 QUP_OPERATIONAL_MASK;                             /* OFFSET 0x28 */
   const uint32 _FILL1[1];                                  /* OFFSET 0x2c */
   uint32 QUP_HW_VERSION;                                   /* OFFSET 0x30 */
   const uint32 _FILL2[51];                                 /* OFFSET 0x34 */
   uint32 QUP_MX_OUTPUT_COUNT;                              /* OFFSET 0x100 */
   uint32 QUP_MX_OUTPUT_CNT_CURRENT;                        /* OFFSET 0x104 */
   uint32 QUP_OUTPUT_DEBUG;                                 /* OFFSET 0x108 */
   uint32 QUP_OUTPUT_FIFO_WORD_CNT;                         /* OFFSET 0x10c */
   uint32 QUP_OUTPUT_FIFO0;                                 /* OFFSET 0x110 */
   const uint32 _FILL3[15];                                 /* OFFSET 0x114 */
   uint32 QUP_MX_WRITE_COUNT;                               /* OFFSET 0x150 */
   uint32 QUP_MX_WRITE_CNT_CURRENT;                         /* OFFSET 0x154 */
   const uint32 _FILL4[42];                                 /* OFFSET 0x158 */
   uint32 QUP_MX_INPUT_COUNT;                               /* OFFSET 0x200 */
   uint32 QUP_MX_INPUT_CNT_CURRENT;                         /* OFFSET 0x204 */
   uint32 QUP_MX_READ_COUNT;                                /* OFFSET 0x208 */
   uint32 QUP_MX_READ_CNT_CURRENT;                          /* OFFSET 0x20c */
   uint32 QUP_INPUT_DEBUG;                                  /* OFFSET 0x210 */
   uint32 QUP_INPUT_FIFO_WORD_CNT;                          /* OFFSET 0x214 */
   uint32 QUP_INPUT_FIFO0;                                  /* OFFSET 0x218 */
   const uint32 _FILL5[57];                                 /* OFFSET 0x21c */
   uint32 SPI_CONFIG;                                       /* OFFSET 0x300 */
   uint32 SPI_IO_CONTROL;                                   /* OFFSET 0x304 */
   uint32 SPI_ERROR_FLAGS;                                  /* OFFSET 0x308 */
   uint32 SPI_ERROR_FLAGS_EN;                               /* OFFSET 0x30c */
   uint32 SPI_DEASSERT_WAIT;                                /* OFFSET 0x310 */
   uint32 SPI_MASTER_LOCAL_ID;                              /* OFFSET 0x314 */
   uint32 SPI_MASTER_COMMAND;                               /* OFFSET 0x318 */
   uint32 SPI_MASTER_STATUS;                                /* OFFSET 0x31c */
   uint32 SPI_CHAR_CFG;                                     /* OFFSET 0x320 */
   uint32 SPI_CHAR_DATA_SPI_CS;                             /* OFFSET 0x324 */
   uint32 SPI_CHAR_DATA_SPI_MOSI;                           /* OFFSET 0x328 */
   uint32 SPI_CHAR_DATA_SPI_MISO;                           /* OFFSET 0x32c */
   const uint32 _FILL6[52];                                 /* OFFSET 0x330 */
   uint32 I2C_MASTER_CLK_CTL;                               /* OFFSET 0x400 */
   uint32 I2C_MASTER_STATUS;                                /* OFFSET 0x404 */
   uint32 I2C_MASTER_CONFIG;                                /* OFFSET 0x408 */
   uint32 I2C_MASTER_BUS_CLEAR;                             /* OFFSET 0x40c */
   uint32 I2C_MASTER_LOCAL_ID;                              /* OFFSET 0x410 */
   uint32 I2C_MASTER_COMMAND;                               /* OFFSET 0x414 */
} QupRegsType;

#define QUP_CONFIG_EN_EXT_OUT_FLAG_BMSK                          0x10000
#define QUP_CONFIG_EN_EXT_OUT_FLAG_SHFT                          0x10
#define QUP_CONFIG_CORE_EXTRA_CLK_ON_EN_BMSK                     0x8000
#define QUP_CONFIG_CORE_EXTRA_CLK_ON_EN_SHFT                     0xf
#define QUP_CONFIG_FIFO_CLK_GATE_EN_BMSK                         0x4000
#define QUP_CONFIG_FIFO_CLK_GATE_EN_SHFT                         0xe
#define QUP_CONFIG_CORE_CLK_ON_EN_BMSK                           0x2000
#define QUP_CONFIG_CORE_CLK_ON_EN_SHFT                           0xd
#define QUP_CONFIG_APP_CLK_ON_EN_BMSK                            0x1000
#define QUP_CONFIG_APP_CLK_ON_EN_SHFT                            0xc
#define QUP_CONFIG_MINI_CORE_BMSK                                0xf00
#define QUP_CONFIG_MINI_CORE_SHFT                                0x8
#define QUP_CONFIG_NO_INPUT_BMSK                                 0x80
#define QUP_CONFIG_NO_INPUT_SHFT                                 0x7
#define QUP_CONFIG_NO_OUTPUT_BMSK                                0x40
#define QUP_CONFIG_NO_OUTPUT_SHFT                                0x6
#define QUP_CONFIG_QUP_HREADY_CTRL_BMSK                          0x20
#define QUP_CONFIG_QUP_HREADY_CTRL_SHFT                          0x5
#define QUP_CONFIG_N_BMSK                                        0x1f
#define QUP_CONFIG_N_SHFT                                        0x0
#define QUP_STATE_I2C_FLUSH_BMSK                                 0x40
#define QUP_STATE_I2C_FLUSH_SHFT                                 0x6
#define QUP_STATE_WAIT_FOR_EOT_BMSK                              0x20
#define QUP_STATE_WAIT_FOR_EOT_SHFT                              0x5
#define QUP_STATE_I2C_MAST_GEN_BMSK                              0x10
#define QUP_STATE_I2C_MAST_GEN_SHFT                              0x4
#define QUP_STATE_SPI_GEN_BMSK                                   0x8
#define QUP_STATE_SPI_GEN_SHFT                                   0x3
#define QUP_STATE_STATE_VALID_BMSK                               0x4
#define QUP_STATE_STATE_VALID_SHFT                               0x2
#define QUP_STATE_STATE_BMSK                                     0x3
#define QUP_STATE_STATE_SHFT                                     0x0
#define QUP_IO_MODES_OUTPUT_BIT_SHIFT_EN_BMSK                    0x10000
#define QUP_IO_MODES_OUTPUT_BIT_SHIFT_EN_SHFT                    0x10
#define QUP_IO_MODES_PACK_EN_BMSK                                0x8000
#define QUP_IO_MODES_PACK_EN_SHFT                                0xf
#define QUP_IO_MODES_UNPACK_EN_BMSK                              0x4000
#define QUP_IO_MODES_UNPACK_EN_SHFT                              0xe
#define QUP_IO_MODES_INPUT_MODE_BMSK                             0x3000
#define QUP_IO_MODES_INPUT_MODE_SHFT                             0xc
#define QUP_IO_MODES_OUTPUT_MODE_BMSK                            0xc00
#define QUP_IO_MODES_OUTPUT_MODE_SHFT                            0xa
#define QUP_IO_MODES_INPUT_FIFO_SIZE_BMSK                        0x380
#define QUP_IO_MODES_INPUT_FIFO_SIZE_SHFT                        0x7
#define QUP_IO_MODES_INPUT_BLOCK_SIZE_BMSK                       0x60
#define QUP_IO_MODES_INPUT_BLOCK_SIZE_SHFT                       0x5
#define QUP_IO_MODES_OUTPUT_FIFO_SIZE_BMSK                       0x1c
#define QUP_IO_MODES_OUTPUT_FIFO_SIZE_SHFT                       0x2
#define QUP_IO_MODES_OUTPUT_BLOCK_SIZE_BMSK                      0x3
#define QUP_IO_MODES_OUTPUT_BLOCK_SIZE_SHFT                      0x0
#define QUP_SW_RESET_QUP_SW_RESET_BMSK                           0xffffffff
#define QUP_SW_RESET_QUP_SW_RESET_SHFT                           0x0
#define QUP_TRANSFER_CANCEL_TRANSFER_CANCEL_ID_BMSK              0xff00
#define QUP_TRANSFER_CANCEL_TRANSFER_CANCEL_ID_SHFT              0x8
#define QUP_TRANSFER_CANCEL_TRANSFER_CANCEL_BMSK                 0x80
#define QUP_TRANSFER_CANCEL_TRANSFER_CANCEL_SHFT                 0x7
#define QUP_OPERATIONAL_NWD_BMSK                                 0x8000
#define QUP_OPERATIONAL_NWD_SHFT                                 0xf
#define QUP_OPERATIONAL_DONE_TOGGLE_BMSK                         0x4000
#define QUP_OPERATIONAL_DONE_TOGGLE_SHFT                         0xe
#define QUP_OPERATIONAL_IN_BLOCK_READ_REQ_BMSK                   0x2000
#define QUP_OPERATIONAL_IN_BLOCK_READ_REQ_SHFT                   0xd
#define QUP_OPERATIONAL_OUT_BLOCK_WRITE_REQ_BMSK                 0x1000
#define QUP_OPERATIONAL_OUT_BLOCK_WRITE_REQ_SHFT                 0xc
#define QUP_OPERATIONAL_MAX_INPUT_DONE_FLAG_BMSK                 0x800
#define QUP_OPERATIONAL_MAX_INPUT_DONE_FLAG_SHFT                 0xb
#define QUP_OPERATIONAL_MAX_OUTPUT_DONE_FLAG_BMSK                0x400
#define QUP_OPERATIONAL_MAX_OUTPUT_DONE_FLAG_SHFT                0xa
#define QUP_OPERATIONAL_INPUT_SERVICE_FLAG_BMSK                  0x200
#define QUP_OPERATIONAL_INPUT_SERVICE_FLAG_SHFT                  0x9
#define QUP_OPERATIONAL_OUTPUT_SERVICE_FLAG_BMSK                 0x100
#define QUP_OPERATIONAL_OUTPUT_SERVICE_FLAG_SHFT                 0x8
#define QUP_OPERATIONAL_INPUT_FIFO_FULL_BMSK                     0x80
#define QUP_OPERATIONAL_INPUT_FIFO_FULL_SHFT                     0x7
#define QUP_OPERATIONAL_OUTPUT_FIFO_FULL_BMSK                    0x40
#define QUP_OPERATIONAL_OUTPUT_FIFO_FULL_SHFT                    0x6
#define QUP_OPERATIONAL_INPUT_FIFO_NOT_EMPTY_BMSK                0x20
#define QUP_OPERATIONAL_INPUT_FIFO_NOT_EMPTY_SHFT                0x5
#define QUP_OPERATIONAL_OUTPUT_FIFO_NOT_EMPTY_BMSK               0x10
#define QUP_OPERATIONAL_OUTPUT_FIFO_NOT_EMPTY_SHFT               0x4
#define QUP_ERROR_FLAGS_OUTPUT_OVER_RUN_ERR_BMSK                 0x20
#define QUP_ERROR_FLAGS_OUTPUT_OVER_RUN_ERR_SHFT                 0x5
#define QUP_ERROR_FLAGS_INPUT_UNDER_RUN_ERR_BMSK                 0x10
#define QUP_ERROR_FLAGS_INPUT_UNDER_RUN_ERR_SHFT                 0x4
#define QUP_ERROR_FLAGS_OUTPUT_UNDER_RUN_ERR_BMSK                0x8
#define QUP_ERROR_FLAGS_OUTPUT_UNDER_RUN_ERR_SHFT                0x3
#define QUP_ERROR_FLAGS_INPUT_OVER_RUN_ERR_BMSK                  0x4
#define QUP_ERROR_FLAGS_INPUT_OVER_RUN_ERR_SHFT                  0x2
#define QUP_ERROR_FLAGS_EN_OUTPUT_OVER_RUN_ERR_EN_BMSK           0x20
#define QUP_ERROR_FLAGS_EN_OUTPUT_OVER_RUN_ERR_EN_SHFT           0x5
#define QUP_ERROR_FLAGS_EN_INPUT_UNDER_RUN_ERR_EN_BMSK           0x10
#define QUP_ERROR_FLAGS_EN_INPUT_UNDER_RUN_ERR_EN_SHFT           0x4
#define QUP_ERROR_FLAGS_EN_OUTPUT_UNDER_RUN_ERR_EN_BMSK          0x8
#define QUP_ERROR_FLAGS_EN_OUTPUT_UNDER_RUN_ERR_EN_SHFT          0x3
#define QUP_ERROR_FLAGS_EN_INPUT_OVER_RUN_ERR_EN_BMSK            0x4
#define QUP_ERROR_FLAGS_EN_INPUT_OVER_RUN_ERR_EN_SHFT            0x2
#define QUP_TEST_CTRL_QUP_TEST_BUS_EN_BMSK                       0x1
#define QUP_TEST_CTRL_QUP_TEST_BUS_EN_SHFT                       0x0
#define QUP_OPERATIONAL_MASK_INPUT_SERVICE_MASK_BMSK             0x200
#define QUP_OPERATIONAL_MASK_INPUT_SERVICE_MASK_SHFT             0x9
#define QUP_OPERATIONAL_MASK_OUTPUT_SERVICE_MASK_BMSK            0x100
#define QUP_OPERATIONAL_MASK_OUTPUT_SERVICE_MASK_SHFT            0x8
#define QUP_HW_VERSION_MAJOR_BMSK                                0xf0000000
#define QUP_HW_VERSION_MAJOR_SHFT                                0x1c
#define QUP_HW_VERSION_MINOR_BMSK                                0xfff0000
#define QUP_HW_VERSION_MINOR_SHFT                                0x10
#define QUP_HW_VERSION_STEP_BMSK                                 0xffff
#define QUP_HW_VERSION_STEP_SHFT                                 0x0
#define QUP_MX_OUTPUT_COUNT_MX_CONFIG_DURING_RUN_BMSK            0x80000000
#define QUP_MX_OUTPUT_COUNT_MX_CONFIG_DURING_RUN_SHFT            0x1f
#define QUP_MX_OUTPUT_COUNT_MX_OUTPUT_COUNT_BMSK                 0xffff
#define QUP_MX_OUTPUT_COUNT_MX_OUTPUT_COUNT_SHFT                 0x0
#define QUP_MX_OUTPUT_CNT_CURRENT_MX_OUTPUT_CNT_CURRENT_BMSK     0xffff
#define QUP_MX_OUTPUT_CNT_CURRENT_MX_OUTPUT_CNT_CURRENT_SHFT     0x0
#define QUP_OUTPUT_DEBUG_OUTPUT_DEBUG_DATA_BMSK                  0xffffffff
#define QUP_OUTPUT_DEBUG_OUTPUT_DEBUG_DATA_SHFT                  0x0
#define QUP_OUTPUT_FIFO_WORD_CNT_OUTPUT_FIFO_WORD_CNT_BMSK       0x1ff
#define QUP_OUTPUT_FIFO_WORD_CNT_OUTPUT_FIFO_WORD_CNT_SHFT       0x0
#define QUP_OUTPUT_FIFO0_OUTPUT_BMSK                             0xffffffff
#define QUP_OUTPUT_FIFO0_OUTPUT_SHFT                             0x0
#define QUP_MX_WRITE_COUNT_MX_CONFIG_DURING_RUN_BMSK             0x80000000
#define QUP_MX_WRITE_COUNT_MX_CONFIG_DURING_RUN_SHFT             0x1f
#define QUP_MX_WRITE_COUNT_MX_WRITE_COUNT_BMSK                   0xffff
#define QUP_MX_WRITE_COUNT_MX_WRITE_COUNT_SHFT                   0x0
#define QUP_MX_WRITE_CNT_CURRENT_MX_WRITE_CNT_CURRENT_BMSK       0xffff
#define QUP_MX_WRITE_CNT_CURRENT_MX_WRITE_CNT_CURRENT_SHFT       0x0
#define QUP_MX_INPUT_COUNT_MX_CONFIG_DURING_RUN_BMSK             0x80000000
#define QUP_MX_INPUT_COUNT_MX_CONFIG_DURING_RUN_SHFT             0x1f
#define QUP_MX_INPUT_COUNT_MX_INPUT_COUNT_BMSK                   0xffff
#define QUP_MX_INPUT_COUNT_MX_INPUT_COUNT_SHFT                   0x0
#define QUP_MX_INPUT_CNT_CURRENT_MX_INPUT_CNT_CURRENT_BMSK       0xffff
#define QUP_MX_INPUT_CNT_CURRENT_MX_INPUT_CNT_CURRENT_SHFT       0x0
#define QUP_MX_READ_COUNT_MX_CONFIG_DURING_RUN_BMSK              0x80000000
#define QUP_MX_READ_COUNT_MX_CONFIG_DURING_RUN_SHFT              0x1f
#define QUP_MX_READ_COUNT_MX_READ_COUNT_BMSK                     0xffff
#define QUP_MX_READ_COUNT_MX_READ_COUNT_SHFT                     0x0
#define QUP_MX_READ_CNT_CURRENT_MX_READ_CNT_CURRENT_BMSK         0xffff
#define QUP_MX_READ_CNT_CURRENT_MX_READ_CNT_CURRENT_SHFT         0x0
#define QUP_INPUT_DEBUG_INPUT_DEBUG_DATA_BMSK                    0xffffffff
#define QUP_INPUT_DEBUG_INPUT_DEBUG_DATA_SHFT                    0x0
#define QUP_INPUT_FIFO_WORD_CNT_INPUT_FIFO_WORD_CNT_BMSK         0x1ff
#define QUP_INPUT_FIFO_WORD_CNT_INPUT_FIFO_WORD_CNT_SHFT         0x0
#define QUP_INPUT_FIFO0_INPUT_BMSK                               0xffffffff
#define QUP_INPUT_FIFO0_INPUT_SHFT                               0x0
#define SPI_CONFIG_HS_MODE_BMSK                                  0x400
#define SPI_CONFIG_HS_MODE_SHFT                                  0xa
#define SPI_CONFIG_INPUT_FIRST_BMSK                              0x200
#define SPI_CONFIG_INPUT_FIRST_SHFT                              0x9
#define SPI_CONFIG_LOOP_BACK_BMSK                                0x100
#define SPI_CONFIG_LOOP_BACK_SHFT                                0x8
#define SPI_CONFIG_SLAVE_OPERATION_BMSK                          0x20
#define SPI_CONFIG_SLAVE_OPERATION_SHFT                          0x5
#define SPI_IO_CONTROL_FORCE_CS_BMSK                             0x800
#define SPI_IO_CONTROL_FORCE_CS_SHFT                             0xb
#define SPI_IO_CONTROL_CLK_IDLE_HIGH_BMSK                        0x400
#define SPI_IO_CONTROL_CLK_IDLE_HIGH_SHFT                        0xa
#define SPI_IO_CONTROL_CLK_ALWAYS_ON_BMSK                        0x200
#define SPI_IO_CONTROL_CLK_ALWAYS_ON_SHFT                        0x9
#define SPI_IO_CONTROL_MX_CS_MODE_BMSK                           0x100
#define SPI_IO_CONTROL_MX_CS_MODE_SHFT                           0x8
#define SPI_IO_CONTROL_CS_N_POLARITY_BMSK                        0xf0
#define SPI_IO_CONTROL_CS_N_POLARITY_SHFT                        0x4
#define SPI_IO_CONTROL_CS_SELECT_BMSK                            0xc
#define SPI_IO_CONTROL_CS_SELECT_SHFT                            0x2
#define SPI_IO_CONTROL_TRISTATE_CS_BMSK                          0x2
#define SPI_IO_CONTROL_TRISTATE_CS_SHFT                          0x1
#define SPI_IO_CONTROL_NO_TRI_STATE_BMSK                         0x1
#define SPI_IO_CONTROL_NO_TRI_STATE_SHFT                         0x0
#define SPI_ERROR_FLAGS_TRANSFER_CANCEL_DONE_BMSK                0x8
#define SPI_ERROR_FLAGS_TRANSFER_CANCEL_DONE_SHFT                0x3
#define SPI_ERROR_FLAGS_TRANSFER_CANCEL_ID_MATCH_BMSK            0x4
#define SPI_ERROR_FLAGS_TRANSFER_CANCEL_ID_MATCH_SHFT            0x2
#define SPI_ERROR_FLAGS_SPI_SLV_CLK_OVER_RUN_ERR_BMSK            0x2
#define SPI_ERROR_FLAGS_SPI_SLV_CLK_OVER_RUN_ERR_SHFT            0x1
#define SPI_ERROR_FLAGS_SPI_SLV_CLK_UNDER_RUN_ERR_BMSK           0x1
#define SPI_ERROR_FLAGS_SPI_SLV_CLK_UNDER_RUN_ERR_SHFT           0x0
#define SPI_ERROR_FLAGS_EN_TRANSFER_CANCEL_DONE_EN_BMSK          0x8
#define SPI_ERROR_FLAGS_EN_TRANSFER_CANCEL_DONE_EN_SHFT          0x3
#define SPI_ERROR_FLAGS_EN_TRANSFER_CANCEL_ID_MATCH_EN_BMSK      0x4
#define SPI_ERROR_FLAGS_EN_TRANSFER_CANCEL_ID_MATCH_EN_SHFT      0x2
#define SPI_ERROR_FLAGS_EN_SPI_SLV_CLK_OVER_RUN_ERR_EN_BMSK      0x2
#define SPI_ERROR_FLAGS_EN_SPI_SLV_CLK_OVER_RUN_ERR_EN_SHFT      0x1
#define SPI_ERROR_FLAGS_EN_SPI_SLV_CLK_UNDER_RUN_ERR_EN_BMSK     0x1
#define SPI_ERROR_FLAGS_EN_SPI_SLV_CLK_UNDER_RUN_ERR_EN_SHFT     0x0
#define SPI_DEASSERT_WAIT_DEASSERT_WAIT_BMSK                     0x3f
#define SPI_DEASSERT_WAIT_DEASSERT_WAIT_SHFT                     0x0
#define SPI_MASTER_LOCAL_ID_EXTENDED_ID_BMSK                     0xff00
#define SPI_MASTER_LOCAL_ID_EXTENDED_ID_SHFT                     0x8
#define SPI_MASTER_LOCAL_ID_LOCAL_ID_BMSK                        0xff
#define SPI_MASTER_LOCAL_ID_LOCAL_ID_SHFT                        0x0
#define SPI_MASTER_COMMAND_RESET_CANCEL_FSM_BMSK                 0x1
#define SPI_MASTER_COMMAND_RESET_CANCEL_FSM_SHFT                 0x0
#define SPI_MASTER_STATUS_CANCEL_FSM_STATE_BMSK                  0x7
#define SPI_MASTER_STATUS_CANCEL_FSM_STATE_SHFT                  0x0
#define SPI_CHAR_CFG_CHAR_STATUS_BMSK                            0xf00
#define SPI_CHAR_CFG_CHAR_STATUS_SHFT                            0x8
#define SPI_CHAR_CFG_DIRECTION_BMSK                              0x10
#define SPI_CHAR_CFG_DIRECTION_SHFT                              0x4
#define SPI_CHAR_CFG_ENABLE_BMSK                                 0x1
#define SPI_CHAR_CFG_ENABLE_SHFT                                 0x0
#define SPI_CHAR_DATA_SPI_CS_CHAR_MODE_BMSK                      0x300
#define SPI_CHAR_DATA_SPI_CS_CHAR_MODE_SHFT                      0x8
#define SPI_CHAR_DATA_SPI_CS_DOUT_DATA_DIN_EXP_BMSK              0xff
#define SPI_CHAR_DATA_SPI_CS_DOUT_DATA_DIN_EXP_SHFT              0x0
#define SPI_CHAR_DATA_SPI_MOSI_CHAR_MODE_BMSK                    0x300
#define SPI_CHAR_DATA_SPI_MOSI_CHAR_MODE_SHFT                    0x8
#define SPI_CHAR_DATA_SPI_MOSI_DOUT_DATA_DIN_EXP_BMSK            0xff
#define SPI_CHAR_DATA_SPI_MOSI_DOUT_DATA_DIN_EXP_SHFT            0x0
#define SPI_CHAR_DATA_SPI_MISO_CHAR_MODE_BMSK                    0x700
#define SPI_CHAR_DATA_SPI_MISO_CHAR_MODE_SHFT                    0x8
#define SPI_CHAR_DATA_SPI_MISO_DOUT_DATA_DIN_EXP_BMSK            0xff
#define SPI_CHAR_DATA_SPI_MISO_DOUT_DATA_DIN_EXP_SHFT            0x0
#define I2C_MASTER_CLK_CTL_SCL_EXT_FORCE_LOW_BMSK                0x10000000
#define I2C_MASTER_CLK_CTL_SCL_EXT_FORCE_LOW_SHFT                0x1c
#define I2C_MASTER_CLK_CTL_SDA_NOISE_REJECTION_BMSK              0xc000000
#define I2C_MASTER_CLK_CTL_SDA_NOISE_REJECTION_SHFT              0x1a
#define I2C_MASTER_CLK_CTL_SCL_NOISE_REJECTION_BMSK              0x3000000
#define I2C_MASTER_CLK_CTL_SCL_NOISE_REJECTION_SHFT              0x18
#define I2C_MASTER_CLK_CTL_HIGH_TIME_DIVIDER_VALUE_BMSK          0xff0000
#define I2C_MASTER_CLK_CTL_HIGH_TIME_DIVIDER_VALUE_SHFT          0x10
#define I2C_MASTER_CLK_CTL_HS_DIVIDER_VALUE_BMSK                 0x700
#define I2C_MASTER_CLK_CTL_HS_DIVIDER_VALUE_SHFT                 0x8
#define I2C_MASTER_CLK_CTL_FS_DIVIDER_VALUE_BMSK                 0xff
#define I2C_MASTER_CLK_CTL_FS_DIVIDER_VALUE_SHFT                 0x0
#define I2C_MASTER_STATUS_CANCEL_FSM_STATE_BMSK                  0x70000000
#define I2C_MASTER_STATUS_CANCEL_FSM_STATE_SHFT                  0x1c
#define I2C_MASTER_STATUS_I2C_SCL_BMSK                           0x8000000
#define I2C_MASTER_STATUS_I2C_SCL_SHFT                           0x1b
#define I2C_MASTER_STATUS_I2C_SDA_BMSK                           0x4000000
#define I2C_MASTER_STATUS_I2C_SDA_SHFT                           0x1a
#define I2C_MASTER_STATUS_INVALID_READ_SEQ_BMSK                  0x2000000
#define I2C_MASTER_STATUS_INVALID_READ_SEQ_SHFT                  0x19
#define I2C_MASTER_STATUS_INVALID_READ_ADDR_BMSK                 0x1000000
#define I2C_MASTER_STATUS_INVALID_READ_ADDR_SHFT                 0x18
#define I2C_MASTER_STATUS_INVALID_TAG_BMSK                       0x800000
#define I2C_MASTER_STATUS_INVALID_TAG_SHFT                       0x17
#define I2C_MASTER_STATUS_INPUT_FSM_STATE_BMSK                   0x700000
#define I2C_MASTER_STATUS_INPUT_FSM_STATE_SHFT                   0x14
#define I2C_MASTER_STATUS_OUTPUT_FSM_STATE_BMSK                  0xf0000
#define I2C_MASTER_STATUS_OUTPUT_FSM_STATE_SHFT                  0x10
#define I2C_MASTER_STATUS_CLK_STATE_BMSK                         0xe000
#define I2C_MASTER_STATUS_CLK_STATE_SHFT                         0xd
#define I2C_MASTER_STATUS_DATA_STATE_BMSK                        0x1c00
#define I2C_MASTER_STATUS_DATA_STATE_SHFT                        0xa
#define I2C_MASTER_STATUS_BUS_MASTER_BMSK                        0x200
#define I2C_MASTER_STATUS_BUS_MASTER_SHFT                        0x9
#define I2C_MASTER_STATUS_BUS_ACTIVE_BMSK                        0x100
#define I2C_MASTER_STATUS_BUS_ACTIVE_SHFT                        0x8
#define I2C_MASTER_STATUS_FAILED_BMSK                            0xc0
#define I2C_MASTER_STATUS_FAILED_SHFT                            0x6
#define I2C_MASTER_STATUS_INVALID_WRITE_BMSK                     0x20
#define I2C_MASTER_STATUS_INVALID_WRITE_SHFT                     0x5
#define I2C_MASTER_STATUS_ARB_LOST_BMSK                          0x10
#define I2C_MASTER_STATUS_ARB_LOST_SHFT                          0x4
#define I2C_MASTER_STATUS_PACKET_NACKED_BMSK                     0x8
#define I2C_MASTER_STATUS_PACKET_NACKED_SHFT                     0x3
#define I2C_MASTER_STATUS_BUS_ERROR_BMSK                         0x4
#define I2C_MASTER_STATUS_BUS_ERROR_SHFT                         0x2
#define I2C_MASTER_STATUS_TRANSFER_CANCEL_DONE_BMSK              0x2
#define I2C_MASTER_STATUS_TRANSFER_CANCEL_DONE_SHFT              0x1
#define I2C_MASTER_STATUS_TRANSFER_CANCEL_ID_MATCH_BMSK          0x1
#define I2C_MASTER_STATUS_TRANSFER_CANCEL_ID_MATCH_SHFT          0x0
#define I2C_MASTER_CONFIG_EN_VERSION_TWO_TAG_BMSK                0x1
#define I2C_MASTER_CONFIG_EN_VERSION_TWO_TAG_SHFT                0x0
#define I2C_MASTER_BUS_CLEAR_CLEAR_BMSK                          0x1
#define I2C_MASTER_BUS_CLEAR_CLEAR_SHFT                          0x0
#define I2C_MASTER_LOCAL_ID_LOCAL_ID_BMSK                        0xff
#define I2C_MASTER_LOCAL_ID_LOCAL_ID_SHFT                        0x0
#define I2C_MASTER_COMMAND_RESET_CANCEL_FSM_BMSK                 0x1
#define I2C_MASTER_COMMAND_RESET_CANCEL_FSM_SHFT                 0x0
#endif // __QUPREGS_H_
