#ifndef __QUPEREGS_H__
#define __QUPEREGS_H__

/* ===========================================================================

FILE:   HALqupeHWIO.h

DESCRIPTION:
    This file contains register offset as well as registe field offsets
    and widths definitions.

==============================================================================

        Edit History

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/buses/i2c/src/dev/QupeRegs.h#1 $

 When     Who    What, where, why
-------- ---    -----------------------------------------------------------
06/16/12 LK     Created

===========================================================================
        Copyright c 2012 Qualcomm Technologies Incorporated.
            All Rights Reserved.
            Qualcomm Proprietary/GTDR

=========================================================================== */


/*------------------------------------------------------------------------------
 * MODULE: qup
 *------------------------------------------------------------------------------*/
#define QUPREGS_OFFSET_FROM_BLSP                    0
#define I2CREGS_OFFSET_FROM_BLSP                    0x400

typedef volatile struct QupRegs {
   uint32 CONFIG;                          /* OFFSET 0x0  - 0x03 */
   uint32 STATE;                           /* OFFSET 0x04 - 0x07 */
   uint32 IO_MODES;                        /* OFFSET 0x08 - 0x0B */
   uint32 SW_RESET;                        /* OFFSET 0x0C - 0x0F */
   uint32 _FILL0[2];                       /* OFFSET 0x10 - 0x17 */
   uint32 OPERATIONAL;                     /* OFFSET 0x18 - 0x1B */
   uint32 ERROR_FLAGS;                     /* OFFSET 0x1C - 0x1F */
   uint32 ERROR_FLAGS_EN;                  /* OFFSET 0x20 - 0x23 */
   uint32 TEST_CTRL;                       /* OFFSET 0x24 - 0x27 */
   uint32 OPERATIONAL_MASK;                /* OFFSET 0x28 - 0x2B */
   uint32 _FILL1;                          /* OFFSET 0x2C - 0x2F */
   uint32 HW_VERSION;                      /* OFFSET 0x30 - 0x33 */
   uint32 _FILL2[51];                      /* OFFSET 0x34 - 0xFF */
   uint32 MX_OUTPUT_COUNT;                 /* OFFSET 0x100 - 0x103 */
   uint32 MX_OUTPUT_CNT_CURRENT;           /* OFFSET 0x104 - 0x107 */
   uint32 OUTPUT_DEBUG;                    /* OFFSET 0x108 - 0x10B */
   uint32 OUTPUT_FIFO_WORD_CNT;            /* OFFSET 0x10C - 0x10F */
   uint32 OUTPUT_FIFO0;                    /* OFFSET 0x110 - 0x113 */
   uint32 _FILL3[15];                      /* OFFSET 0x114 - 0x14F */
   uint32 MX_WRITE_COUNT;                  /* OFFSET 0x150 - 0x153 */
   uint32 MX_WRITE_CNT_CURRENT;            /* OFFSET 0x154 - 0x157 */
   uint32 _FILL4[42];                      /* OFFSET 0x158 - 0x1FF */
   uint32 MX_INPUT_COUNT;                  /* OFFSET 0x200 - 0x203 */
   uint32 MX_INPUT_CNT_CURRENT;            /* OFFSET 0x204 - 0x207 */
   uint32 MX_READ_COUNT;                   /* OFFSET 0x208 - 0x20B */
   uint32 MX_READ_CNT_CURRENT;             /* OFFSET 0x20C - 0x20F */
   uint32 INPUT_DEBUG;                     /* OFFSET 0x210 - 0x213 */
   uint32 INPUT_FIFO_WORD_CNT;             /* OFFSET 0x214 - 0x217 */
   uint32 INPUT_FIFO0;                     /* OFFSET 0x218 - 0x21B */
   uint32 _FILL5[57];                      /* OFFSET 0x21C - 0x2FF */
} QupRegsType;

typedef volatile struct I2cRegs {
   uint32 CLK_CTL;                          /* OFFSET 0x400 - 0x403 */
   uint32 STATUS;                           /* OFFSET 0x404 - 0x407 */
   uint32 CONFIG;                           /* OFFSET 0x408 - 0x40B */
   uint32 BUS_CLEAR;                        /* OFFSET 0x40C - 0x40F */
   uint32 LOCAL_ID;                         /* OFFSET 0x410 - 0x403 */
} I2cRegsType;

/**  QUPe run states.                                                        */
typedef enum QUPE_RunStateType
{
   /**
      Mini Core and related logic is held in reset state.
    */
   QUPE_RUNSTATE_RESET   = 0 ,

   /**
      Mini Core and related logic is released from reset and enters
      the run state.
    */
   QUPE_RUNSTATE_RUN     = 1 ,

   /**
      Pause state.
    */
   QUPE_RUNSTATE_PAUSE   = 3 ,

   /**
      State not valid.
    */
   QUPE_RUNSTATE_INVALID  = 0xFF ,

   QUPE_RUNSTATE_EXTEND  = 0x7FFFFFFF
} QUPE_RunStateType;

/** @enum QUPE_RunStateType QUPe run state.                       */
typedef enum QUPE_RunStateCmdType
{
   QUPE_RUNSTATE_CMD_RESET   = 0x2  , /**<Go to Reset State.          */
   QUPE_RUNSTATE_CMD_RUN     = 0x1  , /**<Go to RUN state.            */
   QUPE_RUNSTATE_CMD_PAUSE   = 0x3  , /**<Go to pause state.          */

   QUPE_RUNSTATE_CMD_EXTEND  = 0x7FFFFFFF
} QUPE_RunStateCmdType;


/** The QUP operational state.                                               */
typedef enum QUPE_OpStateType
{
   /**
      Notify When Done(NWD). Read only status bit which when set indicates
      a NWD acknowledgement is outstanding. This bit is set by QUP hw when
      NWD is signaled from BAM. This bit is cleared by QUP hw when NWD
      request is acknowledeg by assertion of done_toggle. This bit is
      always unconditionally cleared in RESET_STATE.
    */
   QUPE_OPSTATE_NWD             = 0x8000,
   /**
      Read only status bit which provides the current state
      of the side-band done_toggle signal sent to BAM. At
      each NWD acknowledgement, this bit toggles.
    */
   QUPE_OPSTATE_DONE_TOGGLE      = 0x4000,

   /**
            Read only. When set indicates that the input FIFO has
            BLOCK_SIZE_INPUT data ready to be read.
     */
   QUPE_OPSTATE_IN_BLOCK_READ_REQ     = 0x2000,

   /**
         Read only. When set indicates that the output FIFO needs
         BLOCK_SIZE_OUTPUT data to be written.
    */
   QUPE_OPSTATE_OUT_BLOCK_WRITE_REQ     = 0x1000,

   /**
      This flag is set by hardware whenever the programmed
      QUP_MX_INPUT_COUNT value is reached. This can be used
      in conjuction with the qup_input_service_irq assertion
      in Block_Mode and Data_Mover_Mode.
    */
   QUPE_OPSTATE_MAX_INPUT_DONE     = 0x800,

   /**
      This flag is set by whenever whenever the programmed
      QUP_MX_OUTPUT_COUNT value is reached. This can be used
      in conjuction with the qup_output_service_irq assertion in
      Block_Mode and Data_Mover_Mode.
    */
   QUPE_OPSTATE_MAX_OUTPUT_DONE    = 0x400,

   /**
      When set by hardware, indicates QUP Input FIFO has
      BLOCK_SIZE_INPUT amount of data ready for reading. At
      the point in time this bit is set, the hardware also asserts
      qup_input_service_irq. Valid only in Block_Mode.
     */
   QUPE_OPSTATE_INPUT_SERVICE      = 0x200,

   /**
      When set by hardware, indicates QUP Output FIFO needs
      BLOCK_SIZE_OUTPUT amount of data to be written. At
      the point in time this bit is set, the hardware also asserts
      qup_output_service_irq. Valid only in Block_Mode.
    */
   QUPE_OPSTATE_OUTPUT_SERVICE     = 0x100,

   /**
      When set, indicates the input FIFO is full and
      can accept no more data from the QUP Shift Register.
     Ready only.*/
   QUPE_OPSTATE_INPUT_FIFO_FULL      = 0x80,

   /**
      When set, indicates the output FIFO is full and
      can accept no more CRIF writes.
     Ready only.*/
   QUPE_OPSTATE_OUTPUT_FIFO_FULL     = 0x40,

   /**
      Read Only. When set, indicates the input FIFO has at least
      one value in it to be read. When clear, indicates the input
      FIFO is empty.Under FIFO_Mode, the
      qup_input_service_irq may be asserted when this bit
      changes from zero to one.
    */
   QUPE_OPSTATE_INPUT_FIFO_NOT_EMPTY = 0x20,

   /**
      Read Only. When set, indicates the output FIFO has at
      least one value in it to be shifted out. When clear, indicates
      the output FIFO is empty. Under FIFO_Mode, the
      qup_output_service_irq. is asserted when this bit
      changes from one to zero.
     */
   QUPE_OPSTATE_OUTPUT_FIFO_NOT_EMPTY = 0x10,

   /**
      This is a field mask with all flags set.
     */
   QUPE_OPSTATE_FIELD     = QUPE_OPSTATE_NWD                  |
                            QUPE_OPSTATE_DONE_TOGGLE          |
                            QUPE_OPSTATE_MAX_INPUT_DONE       |
                            QUPE_OPSTATE_MAX_OUTPUT_DONE      |
                            QUPE_OPSTATE_INPUT_SERVICE        |
                            QUPE_OPSTATE_OUTPUT_SERVICE       |
                            QUPE_OPSTATE_INPUT_FIFO_FULL      |
                            QUPE_OPSTATE_OUTPUT_FIFO_FULL     |
                            QUPE_OPSTATE_INPUT_FIFO_NOT_EMPTY |
                            QUPE_OPSTATE_OUTPUT_FIFO_NOT_EMPTY,

   QUPE_OPSTATE_EXTEND  = 0x7FFFFFFF
} QUPE_OpStateType;


/** QUPe error state.                                                        */
typedef enum QUPE_ErrStateType
{
   /**
      Indicates the output FIFO was full when a CRIF write was
      attempted to the FIFO. The write is discarded.
     */
   QUPE_ERRSTATE_BAM_TRANS_ERROR  = 0x80, 

   /**
      Indicates the output FIFO was full when a CRIF write was
      attempted to the FIFO. The write is discarded.
     */
   QUPE_ERRSTATE_OUTPUT_OVER_RUN  = 0x20,

   /**
      Indicates the input FIFO was empty when a CRIF read was
      attempted to the FIFO. The read value returned is
      indeterminate.
     */
   QUPE_ERRSTATE_INPUT_UNDER_RUN  = 0x10,

   /**
      Indicates the output FIFO was empty when an output shift
      operation required a value.
     */
   QUPE_ERRSTATE_OUTPUT_UNDER_RUN = 0x8,

   /**
      Indicates the input FIFO was full when an input shift
      operation provided a new value. When this happens, the
      new value is discarded.
     */
   QUPE_ERRSTATE_INPUT_OVER_RUN   = 0x4,

   /**
      This is a field mask with all flags set.
     */
   QUPE_ERRSTATE_FIELD = QUPE_ERRSTATE_BAM_TRANS_ERROR  |
                         QUPE_ERRSTATE_OUTPUT_OVER_RUN  |
                         QUPE_ERRSTATE_INPUT_UNDER_RUN  |
                         QUPE_ERRSTATE_OUTPUT_UNDER_RUN |
                         QUPE_ERRSTATE_INPUT_OVER_RUN,

   QUPE_ERRSTATE_EXTEND  = 0x7FFFFFFF
} QUPE_ErrStateType;


/** Operation state.                                                         */
typedef enum I2C_OpStatusType
{
   I2C_OP_STATUS_INVALID_READ_SEQ    = 0x2000000,
   I2C_OP_STATUS_INVALID_READ_ADDR   = 0x1000000,
   I2C_OP_STATUS_INVALID_TAG         = 0x800000,
   I2C_OP_STATUS_FAILED              = 0xC0,
   I2C_OP_STATUS_INVALID_WRITE       = 0x20,
   I2C_OP_STATUS_ARB_LOST            = 0x10,
   I2C_OP_STATUS_PACKET_NACKED       = 0x08,
   I2C_OP_STATUS_BUS_ERROR           = 0x04,

   I2C_OP_STATUS_ERROR_FIELD =
                  I2C_OP_STATUS_INVALID_READ_SEQ |
                  I2C_OP_STATUS_INVALID_READ_ADDR|
                  I2C_OP_STATUS_INVALID_TAG      |
                  I2C_OP_STATUS_FAILED           |
                  I2C_OP_STATUS_INVALID_WRITE    |
                  I2C_OP_STATUS_ARB_LOST         |
                  I2C_OP_STATUS_PACKET_NACKED    |
                  I2C_OP_STATUS_BUS_ERROR        ,

   I2C_OP_STATUS_FLAG_EXTEND  = 0x7FFFFFFF
} I2C_OpStatusType;



#define QUP_CONFIG_RMSK                                                                        0xf3fdf
//#define QUP_CONFIG_EN_EXT_STATE_VALID_BMSK                                                     0x80000
//#define QUP_CONFIG_EN_EXT_STATE_VALID_SHFT                                                        0x13
#define QUP_CONFIG_EN_EXT_BLOCK_RD_BMSK                                                        0x40000
#define QUP_CONFIG_EN_EXT_BLOCK_RD_SHFT                                                           0x12
#define QUP_CONFIG_EN_EXT_BLOCK_WRT_BMSK                                                       0x20000
#define QUP_CONFIG_EN_EXT_BLOCK_WRT_SHFT                                                          0x11
#define QUP_CONFIG_EN_EXT_OUT_FLAG_BMSK                                                        0x10000
#define QUP_CONFIG_EN_EXT_OUT_FLAG_SHFT                                                           0x10
#define QUP_CONFIG_CORE_CLK_ON_EN_BMSK                                                          0x2000
#define QUP_CONFIG_CORE_CLK_ON_EN_SHFT                                                             0xd
#define QUP_CONFIG_APP_CLK_ON_EN_BMSK                                                           0x1000
#define QUP_CONFIG_APP_CLK_ON_EN_SHFT                                                              0xc
#define QUP_CONFIG_MINI_CORE_BMSK                                                                0xf00
#define QUP_CONFIG_MINI_CORE_SHFT                                                                  0x8
#define QUP_CONFIG_NO_INPUT_BMSK                                                                  0x80
#define QUP_CONFIG_NO_INPUT_SHFT                                                                   0x7
#define QUP_CONFIG_NO_OUTPUT_BMSK                                                                 0x40
#define QUP_CONFIG_NO_OUTPUT_SHFT                                                                  0x6
#define QUP_CONFIG_N_BMSK                                                                         0x1f
#define QUP_CONFIG_N_SHFT                                                                          0x0

/** Mini Cores List.                                                         */
typedef enum QUPE_MiniCoreType
{
   QUPE_MINI_CORE_FLD_MIN                  = 0  , /**<Smallest field value. */
   QUPE_MINI_CORE_NULL                     = 0  , /**<NULL core.            */
   QUPE_MINI_CORE_SPI                      = 1  , /**<SPI core.             */
   QUPE_MINI_CORE_I2C_MASTER              = 0x2, /**<I2C master controller.*/
   QUPE_MINI_CORE_FLD_MAX                  = 0x2, /**<Biggest field value.  */

   QUPE_MINI_CORE_EXTEND           = 0x7FFFFFFF
} QUPE_MiniCoreType;

#define QUPE_CONFIG_N_8BITS                 7 // zero based


#define QUP_STATE_RMSK                                                                            0x1f
#define QUP_STATE_I2C_FLUSH_BMSK                                                                  0x40
#define QUP_STATE_I2C_FLUSH_SHFT                                                                   0x6
#define QUP_STATE_I2C_MAST_GEN_BMSK                                                               0x10
#define QUP_STATE_I2C_MAST_GEN_SHFT                                                                0x4
#define QUP_STATE_SPI_GEN_BMSK                                                                     0x8
#define QUP_STATE_SPI_GEN_SHFT                                                                     0x3
#define QUP_STATE_STATE_VALID_BMSK                                                                 0x4
#define QUP_STATE_STATE_VALID_SHFT                                                                 0x2
#define QUP_STATE_STATE_BMSK                                                                       0x3
#define QUP_STATE_STATE_SHFT                                                                       0x0

#define QUP_IO_MODES_RMSK                                                                      0x1ffff
#define QUP_IO_MODES_OUTPUT_BIT_SHIFT_EN_BMSK                                                  0x10000
#define QUP_IO_MODES_OUTPUT_BIT_SHIFT_EN_SHFT                                                     0x10
#define QUP_IO_MODES_PACK_EN_BMSK                                                               0x8000
#define QUP_IO_MODES_PACK_EN_SHFT                                                                  0xf
#define QUP_IO_MODES_UNPACK_EN_BMSK                                                             0x4000
#define QUP_IO_MODES_UNPACK_EN_SHFT                                                                0xe
#define QUP_IO_MODES_INPUT_MODE_BMSK                                                            0x3000
#define QUP_IO_MODES_INPUT_MODE_SHFT                                                               0xc
#define QUP_IO_MODES_OUTPUT_MODE_BMSK                                                            0xc00
#define QUP_IO_MODES_OUTPUT_MODE_SHFT                                                              0xa
#define QUP_IO_MODES_INPUT_FIFO_SIZE_BMSK                                                        0x380
#define QUP_IO_MODES_INPUT_FIFO_SIZE_SHFT                                                          0x7
#define QUP_IO_MODES_INPUT_BLOCK_SIZE_BMSK                                                        0x60
#define QUP_IO_MODES_INPUT_BLOCK_SIZE_SHFT                                                         0x5
#define QUP_IO_MODES_OUTPUT_FIFO_SIZE_BMSK                                                        0x1c
#define QUP_IO_MODES_OUTPUT_FIFO_SIZE_SHFT                                                         0x2
#define QUP_IO_MODES_OUTPUT_BLOCK_SIZE_BMSK                                                        0x3
#define QUP_IO_MODES_OUTPUT_BLOCK_SIZE_SHFT                                                        0x0

/** QUPe IO Modes.                                                           */
typedef enum QUPE_IoModeType{
   QUPE_IO_MODE_FLD_MIN = 0, /**<IO mode min value.           */
   QUPE_IO_MODE_FIFO    = 0, /**<FIFO input-output mode.      */
   QUPE_IO_MODE_BLOCK   = 1, /**<BLOCK input-output mode.     */
   QUPE_IO_MODE_BAM     = 3, /**<DATA MOVER input-output mode.*/
   QUPE_IO_MODE_FLD_MAX = 3, /**<IO mode max value.           */

   QUPE_IO_MODE_EXTEND           = 0x7FFFFFFF
} QUPE_IoModeType;



#define QUP_SW_RESET_RMSK                                                                   0xffffffff
#define QUP_SW_RESET_QUP_SW_RESET_BMSK                                                      0xffffffff
#define QUP_SW_RESET_QUP_SW_RESET_SHFT                                                             0x0

#define QUP_OPERATIONAL_RMSK                                                                    0xfff0
#define QUP_OPERATIONAL_NWD_BMSK                                                                0x8000
#define QUP_OPERATIONAL_NWD_SHFT                                                                   0xf
#define QUP_OPERATIONAL_DONE_TOGGLE_BMSK                                                        0x4000
#define QUP_OPERATIONAL_DONE_TOGGLE_SHFT                                                           0xe
#define QUP_OPERATIONAL_IN_BLOCK_READ_REQ_BMSK                                                  0x2000
#define QUP_OPERATIONAL_IN_BLOCK_READ_REQ_SHFT                                                     0xd
#define QUP_OPERATIONAL_OUT_BLOCK_WRITE_REQ_BMSK                                                0x1000
#define QUP_OPERATIONAL_OUT_BLOCK_WRITE_REQ_SHFT                                                   0xc
#define QUP_OPERATIONAL_MAX_INPUT_DONE_FLAG_BMSK                                                 0x800
#define QUP_OPERATIONAL_MAX_INPUT_DONE_FLAG_SHFT                                                   0xb
#define QUP_OPERATIONAL_MAX_OUTPUT_DONE_FLAG_BMSK                                                0x400
#define QUP_OPERATIONAL_MAX_OUTPUT_DONE_FLAG_SHFT                                                  0xa
#define QUP_OPERATIONAL_INPUT_SERVICE_FLAG_BMSK                                                  0x200
#define QUP_OPERATIONAL_INPUT_SERVICE_FLAG_SHFT                                                    0x9
#define QUP_OPERATIONAL_OUTPUT_SERVICE_FLAG_BMSK                                                 0x100
#define QUP_OPERATIONAL_OUTPUT_SERVICE_FLAG_SHFT                                                   0x8
#define QUP_OPERATIONAL_INPUT_FIFO_FULL_BMSK                                                      0x80
#define QUP_OPERATIONAL_INPUT_FIFO_FULL_SHFT                                                       0x7
#define QUP_OPERATIONAL_OUTPUT_FIFO_FULL_BMSK                                                     0x40
#define QUP_OPERATIONAL_OUTPUT_FIFO_FULL_SHFT                                                      0x6
#define QUP_OPERATIONAL_INPUT_FIFO_NOT_EMPTY_BMSK                                                 0x20
#define QUP_OPERATIONAL_INPUT_FIFO_NOT_EMPTY_SHFT                                                  0x5
#define QUP_OPERATIONAL_OUTPUT_FIFO_NOT_EMPTY_BMSK                                                0x10
#define QUP_OPERATIONAL_OUTPUT_FIFO_NOT_EMPTY_SHFT                                                 0x4

#define QUP_ERROR_FLAGS_RMSK                                                                      0x7c
#define QUP_ERROR_FLAGS_TIME_OUT_ERR_BMSK                                                         0x40
#define QUP_ERROR_FLAGS_TIME_OUT_ERR_SHFT                                                          0x6
#define QUP_ERROR_FLAGS_OUTPUT_OVER_RUN_ERR_BMSK                                                  0x20
#define QUP_ERROR_FLAGS_OUTPUT_OVER_RUN_ERR_SHFT                                                   0x5
#define QUP_ERROR_FLAGS_INPUT_UNDER_RUN_ERR_BMSK                                                  0x10
#define QUP_ERROR_FLAGS_INPUT_UNDER_RUN_ERR_SHFT                                                   0x4
#define QUP_ERROR_FLAGS_OUTPUT_UNDER_RUN_ERR_BMSK                                                  0x8
#define QUP_ERROR_FLAGS_OUTPUT_UNDER_RUN_ERR_SHFT                                                  0x3
#define QUP_ERROR_FLAGS_INPUT_OVER_RUN_ERR_BMSK                                                    0x4
#define QUP_ERROR_FLAGS_INPUT_OVER_RUN_ERR_SHFT                                                    0x2

#define QUP_ERROR_FLAGS_EN_RMSK                                                                   0x7c
#define QUP_ERROR_FLAGS_EN_TIME_OUT_ERR_EN_BMSK                                                   0x40
#define QUP_ERROR_FLAGS_EN_TIME_OUT_ERR_EN_SHFT                                                    0x6
#define QUP_ERROR_FLAGS_EN_OUTPUT_OVER_RUN_ERR_EN_BMSK                                            0x20
#define QUP_ERROR_FLAGS_EN_OUTPUT_OVER_RUN_ERR_EN_SHFT                                             0x5
#define QUP_ERROR_FLAGS_EN_INPUT_UNDER_RUN_ERR_EN_BMSK                                            0x10
#define QUP_ERROR_FLAGS_EN_INPUT_UNDER_RUN_ERR_EN_SHFT                                             0x4
#define QUP_ERROR_FLAGS_EN_OUTPUT_UNDER_RUN_ERR_EN_BMSK                                            0x8
#define QUP_ERROR_FLAGS_EN_OUTPUT_UNDER_RUN_ERR_EN_SHFT                                            0x3
#define QUP_ERROR_FLAGS_EN_INPUT_OVER_RUN_ERR_EN_BMSK                                              0x4
#define QUP_ERROR_FLAGS_EN_INPUT_OVER_RUN_ERR_EN_SHFT                                              0x2

#define QUP_TEST_CTRL_RMSK                                                                         0x1
#define QUP_TEST_CTRL_QUP_TEST_BUS_EN_BMSK                                                         0x1
#define QUP_TEST_CTRL_QUP_TEST_BUS_EN_SHFT                                                         0x0

#define QUP_OPERATIONAL_MASK_RMSK                                                                0x300
#define QUP_OPERATIONAL_MASK_INPUT_SERVICE_MASK_BMSK                                             0x200
#define QUP_OPERATIONAL_MASK_INPUT_SERVICE_MASK_SHFT                                               0x9
#define QUP_OPERATIONAL_MASK_OUTPUT_SERVICE_MASK_BMSK                                            0x100
#define QUP_OPERATIONAL_MASK_OUTPUT_SERVICE_MASK_SHFT                                              0x8

#define QUP_HW_VERSION_RMSK                                                                 0xffffffff
#define QUP_HW_VERSION_QUP_HW_VERSION_BMSK                                                  0xffffffff
#define QUP_HW_VERSION_QUP_HW_VERSION_SHFT                                                         0x0

#define QUP_MX_OUTPUT_COUNT_RMSK                                                                0xffff
#define QUP_MX_OUTPUT_COUNT_MX_OUTPUT_COUNT_BMSK                                                0xffff
#define QUP_MX_OUTPUT_COUNT_MX_OUTPUT_COUNT_SHFT                                                   0x0

#define QUP_MX_OUTPUT_CNT_CURRENT_RMSK                                                          0xffff
#define QUP_MX_OUTPUT_CNT_CURRENT_MX_OUTPUT_CNT_CURRENT_BMSK                                    0xffff
#define QUP_MX_OUTPUT_CNT_CURRENT_MX_OUTPUT_CNT_CURRENT_SHFT                                       0x0

#define QUP_OUTPUT_DEBUG_RMSK                                                               0xffffffff
#define QUP_OUTPUT_DEBUG_OUTPUT_DEBUG_DATA_BMSK                                             0xffffffff
#define QUP_OUTPUT_DEBUG_OUTPUT_DEBUG_DATA_SHFT                                                    0x0

#define QUP_OUTPUT_FIFO_WORD_CNT_RMSK                                                            0x1ff
#define QUP_OUTPUT_FIFO_WORD_CNT_OUTPUT_FIFO_WORD_CNT_BMSK                                       0x1ff
#define QUP_OUTPUT_FIFO_WORD_CNT_OUTPUT_FIFO_WORD_CNT_SHFT                                         0x0

#define QUP_OUTPUT_FIFO0_RMSK                                                               0xffffffff
#define QUP_OUTPUT_FIFO0_OUTPUT_BMSK                                                        0xffffffff
#define QUP_OUTPUT_FIFO0_OUTPUT_SHFT                                                               0x0

#define QUP_MX_WRITE_COUNT_RMSK                                                                 0xffff
#define QUP_MX_WRITE_COUNT_MX_WRITE_COUNT_BMSK                                                  0xffff
#define QUP_MX_WRITE_COUNT_MX_WRITE_COUNT_SHFT                                                     0x0

#define QUP_MX_WRITE_CNT_CURRENT_RMSK                                                           0xffff
#define QUP_MX_WRITE_CNT_CURRENT_MX_WRITE_CNT_CURRENT_BMSK                                      0xffff
#define QUP_MX_WRITE_CNT_CURRENT_MX_WRITE_CNT_CURRENT_SHFT                                         0x0

#define QUP_MX_INPUT_COUNT_RMSK                                                                 0xffff
#define QUP_MX_INPUT_COUNT_MX_INPUT_COUNT_BMSK                                                  0xffff
#define QUP_MX_INPUT_COUNT_MX_INPUT_COUNT_SHFT                                                     0x0

#define QUP_MX_INPUT_CNT_CURRENT_RMSK                                                           0xffff
#define QUP_MX_INPUT_CNT_CURRENT_MX_INPUT_CNT_CURRENT_BMSK                                      0xffff
#define QUP_MX_INPUT_CNT_CURRENT_MX_INPUT_CNT_CURRENT_SHFT                                         0x0

#define QUP_MX_READ_COUNT_RMSK                                                                  0xffff
#define QUP_MX_READ_COUNT_MX_READ_COUNT_BMSK                                                    0xffff
#define QUP_MX_READ_COUNT_MX_READ_COUNT_SHFT                                                       0x0

#define QUP_MX_READ_CNT_CURRENT_RMSK                                                            0xffff
#define QUP_MX_READ_CNT_CURRENT_MX_READ_CNT_CURRENT_BMSK                                        0xffff
#define QUP_MX_READ_CNT_CURRENT_MX_READ_CNT_CURRENT_SHFT                                           0x0

#define QUP_INPUT_DEBUG_RMSK                                                                0xffffffff
#define QUP_INPUT_DEBUG_INPUT_DEBUG_DATA_BMSK                                               0xffffffff
#define QUP_INPUT_DEBUG_INPUT_DEBUG_DATA_SHFT                                                      0x0

#define QUP_INPUT_FIFO_WORD_CNT_RMSK                                                             0x1ff
#define QUP_INPUT_FIFO_WORD_CNT_INPUT_FIFO_WORD_CNT_BMSK                                         0x1ff
#define QUP_INPUT_FIFO_WORD_CNT_INPUT_FIFO_WORD_CNT_SHFT                                           0x0

#define QUP_INPUT_FIFO0_RMSK                                                                0xffffffff
#define QUP_INPUT_FIFO0_INPUT_BMSK                                                          0xffffffff
#define QUP_INPUT_FIFO0_INPUT_SHFT                                                                 0x0

#define SPI_CONFIG_RMSK                                                                          0x720
#define SPI_CONFIG_HS_MODE_BMSK                                                                  0x400
#define SPI_CONFIG_HS_MODE_SHFT                                                                    0xa
#define SPI_CONFIG_INPUT_FIRST_BMSK                                                              0x200
#define SPI_CONFIG_INPUT_FIRST_SHFT                                                                0x9
#define SPI_CONFIG_LOOP_BACK_BMSK                                                                0x100
#define SPI_CONFIG_LOOP_BACK_SHFT                                                                  0x8
#define SPI_CONFIG_SLAVE_OPERATION_BMSK                                                           0x20
#define SPI_CONFIG_SLAVE_OPERATION_SHFT                                                            0x5

#define SPI_IO_CONTROL_RMSK                                                                      0xfff
#define SPI_IO_CONTROL_FORCE_CS_BMSK                                                             0x800
#define SPI_IO_CONTROL_FORCE_CS_SHFT                                                               0xb
#define SPI_IO_CONTROL_CLK_IDLE_HIGH_BMSK                                                        0x400
#define SPI_IO_CONTROL_CLK_IDLE_HIGH_SHFT                                                          0xa
#define SPI_IO_CONTROL_CLK_ALWAYS_ON_BMSK                                                        0x200
#define SPI_IO_CONTROL_CLK_ALWAYS_ON_SHFT                                                          0x9
#define SPI_IO_CONTROL_MX_CS_MODE_BMSK                                                           0x100
#define SPI_IO_CONTROL_MX_CS_MODE_SHFT                                                             0x8
#define SPI_IO_CONTROL_CS_N_POLARITY_BMSK                                                         0xf0
#define SPI_IO_CONTROL_CS_N_POLARITY_SHFT                                                          0x4
#define SPI_IO_CONTROL_CS_SELECT_BMSK                                                              0xc
#define SPI_IO_CONTROL_CS_SELECT_SHFT                                                              0x2
#define SPI_IO_CONTROL_TRISTATE_CS_BMSK                                                            0x2
#define SPI_IO_CONTROL_TRISTATE_CS_SHFT                                                            0x1
#define SPI_IO_CONTROL_NO_TRI_STATE_BMSK                                                           0x1
#define SPI_IO_CONTROL_NO_TRI_STATE_SHFT                                                           0x0

#define SPI_ERROR_FLAGS_RMSK                                                                       0x3
#define SPI_ERROR_FLAGS_SPI_SLV_CLK_OVER_RUN_ERR_BMSK                                              0x2
#define SPI_ERROR_FLAGS_SPI_SLV_CLK_OVER_RUN_ERR_SHFT                                              0x1
#define SPI_ERROR_FLAGS_SPI_SLV_CLK_UNDER_RUN_ERR_BMSK                                             0x1
#define SPI_ERROR_FLAGS_SPI_SLV_CLK_UNDER_RUN_ERR_SHFT                                             0x0

#define SPI_ERROR_FLAGS_EN_RMSK                                                                    0x3
#define SPI_ERROR_FLAGS_EN_SPI_SLV_CLK_OVER_RUN_ERR_EN_BMSK                                        0x2
#define SPI_ERROR_FLAGS_EN_SPI_SLV_CLK_OVER_RUN_ERR_EN_SHFT                                        0x1
#define SPI_ERROR_FLAGS_EN_SPI_SLV_CLK_UNDER_RUN_ERR_EN_BMSK                                       0x1
#define SPI_ERROR_FLAGS_EN_SPI_SLV_CLK_UNDER_RUN_ERR_EN_SHFT                                       0x0

#define SPI_DEASSERT_WAIT_RMSK                                                                    0x3f
#define SPI_DEASSERT_WAIT_DEASSERT_WAIT_BMSK                                                      0x3f
#define SPI_DEASSERT_WAIT_DEASSERT_WAIT_SHFT                                                       0x0

#define I2C_CLK_CTL_RMSK                                                              0xfff07ff
#define I2C_CLK_CTL_SDA_NOISE_REJECTION_BMSK                                          0xc000000
#define I2C_CLK_CTL_SDA_NOISE_REJECTION_SHFT                                               0x1a
#define I2C_CLK_CTL_SCL_NOISE_REJECTION_BMSK                                          0x3000000
#define I2C_CLK_CTL_SCL_NOISE_REJECTION_SHFT                                               0x18
#define I2C_CLK_CTL_HIGH_TIME_DIVIDER_VALUE_BMSK                                       0xff0000
#define I2C_CLK_CTL_HIGH_TIME_DIVIDER_VALUE_SHFT                                           0x10
#define I2C_CLK_CTL_HS_DIVIDER_VALUE_BMSK                                                 0x700
#define I2C_CLK_CTL_HS_DIVIDER_VALUE_SHFT                                                   0x8
#define I2C_CLK_CTL_FS_DIVIDER_VALUE_BMSK                                                  0xff
#define I2C_CLK_CTL_FS_DIVIDER_VALUE_SHFT                                                   0x0
#define I2C_CLK_CTL_FS_MIN_DIV_VAL                         3

#define I2C_STATUS_OFFSET                                                              (0x0404)
#define I2C_STATUS_RMSK                                                               0xfbffffc
#define I2C_STATUS_I2C_SCL_BMSK                                                       0x8000000
#define I2C_STATUS_I2C_SCL_SHFT                                                            0x1b
#define I2C_STATUS_I2C_SDA_BMSK                                                       0x4000000
#define I2C_STATUS_I2C_SDA_SHFT                                                            0x1a
#define I2C_STATUS_INVALID_READ_SEQ_BMSK                                              0x2000000
#define I2C_STATUS_INVALID_READ_SEQ_SHFT                                                   0x19
#define I2C_STATUS_INVALID_READ_ADDR_BMSK                                             0x1000000
#define I2C_STATUS_INVALID_READ_ADDR_SHFT                                                  0x18
#define I2C_STATUS_INVALID_TAG_BMSK                                                    0x800000
#define I2C_STATUS_INVALID_TAG_SHFT                                                        0x17
#define I2C_STATUS_INPUT_FSM_STATE_BMSK                                                0x380000
#define I2C_STATUS_INPUT_FSM_STATE_SHFT                                                    0x13
#define I2C_STATUS_OUTPUT_FSM_STATE_BMSK                                                0x70000
#define I2C_STATUS_OUTPUT_FSM_STATE_SHFT                                                   0x10
#define I2C_STATUS_CLK_STATE_BMSK                                                        0xe000
#define I2C_STATUS_CLK_STATE_SHFT                                                           0xd
#define I2C_STATUS_DATA_STATE_BMSK                                                       0x1c00
#define I2C_STATUS_DATA_STATE_SHFT                                                          0xa
#define I2C_STATUS_BUS_MASTER_BMSK                                                        0x200
#define I2C_STATUS_BUS_MASTER_SHFT                                                          0x9
#define I2C_STATUS_BUS_ACTIVE_BMSK                                                        0x100
#define I2C_STATUS_BUS_ACTIVE_SHFT                                                          0x8
#define I2C_STATUS_FAILED_BMSK                                                             0xc0
#define I2C_STATUS_FAILED_SHFT                                                              0x6
#define I2C_STATUS_INVALID_WRITE_BMSK                                                      0x20
#define I2C_STATUS_INVALID_WRITE_SHFT                                                       0x5
#define I2C_STATUS_ARB_LOST_BMSK                                                           0x10
#define I2C_STATUS_ARB_LOST_SHFT                                                            0x4
#define I2C_STATUS_PACKET_NACKED_BMSK                                                       0x8
#define I2C_STATUS_PACKET_NACKED_SHFT                                                       0x3
#define I2C_STATUS_BUS_ERROR_BMSK                                                           0x4
#define I2C_STATUS_BUS_ERROR_SHFT                                                           0x2

#define I2C_CONFIG_RMSK                                                                     0x3
#define I2C_CONFIG_EN_DIRECT_ACCESS_BMSK                                                    0x2
#define I2C_CONFIG_EN_DIRECT_ACCESS_SHFT                                                    0x1
#define I2C_CONFIG_EN_VERSION_TWO_TAG_BMSK                                                  0x1
#define I2C_CONFIG_EN_VERSION_TWO_TAG_SHFT                                                  0x0

typedef enum QUPE_I2cTagType
{
   QUPE_I2C_TAGS_V1          = 0,    /**<I2C V1 TAGS.*/
   QUPE_I2C_TAGS_V2          = 0x1, /**<I2C V2 TAGS.*/
   QUPE_I2C_TAGS_FLD_MAX     = 0x1, /**<I2C V2 TAGS.*/
   QUPE_I2C_TAGS_EXTEND           = 0x7FFFFFFF
} QUPE_I2cTagType;

#define I2C_BUS_CLEAR_OFFSET                                                           (0x040C)
#define I2C_BUS_CLEAR_RMSK                                                                  0x1
#define I2C_BUS_CLEAR_CLEAR_BMSK                                                            0x1
#define I2C_BUS_CLEAR_CLEAR_SHFT                                                            0x0

#endif /* __HAL_HALHWIOQUP_H__ */
