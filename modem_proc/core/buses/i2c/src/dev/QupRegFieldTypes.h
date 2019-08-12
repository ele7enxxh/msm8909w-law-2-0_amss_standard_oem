#ifndef __QUPREGFIELDTYPES_H__
#define __QUPREGFIELDTYPES_H__

/* ===========================================================================

FILE:   QupRegFieldTypes.h

DESCRIPTION:
    This file contains register field types.

==============================================================================

        Edit History

$Header: //components/rel/core.mpss/3.5.c12.3/buses/i2c/src/dev/QupRegFieldTypes.h#1 $

 When     Who    What, where, why
-------- ---    -----------------------------------------------------------
08/25/15 VG     Created

===========================================================================
        Copyright c 2015 Qualcomm Technologies Incorporated.
            All Rights Reserved.
            Qualcomm Proprietary/GTDR

=========================================================================== */


/**  QUPe run states.                                                        */
typedef enum QUP_RunStateType
{
   /**
      Mini Core and related logic is held in reset state.
    */
   QUP_RUNSTATE_RESET   = 0 ,

   /**
      Mini Core and related logic is released from reset and enters
      the run state.
    */
   QUP_RUNSTATE_RUN     = 1 ,

   /**
      Pause state.
    */
   QUP_RUNSTATE_PAUSE   = 3 ,

   /**
      State not valid.
    */
   QUP_RUNSTATE_INVALID  = 0xFF ,

   QUP_RUNSTATE_EXTEND  = 0x7FFFFFFF
} QUP_RunStateType;

/** @enum QUP_RunStateType QUPe run state.                       */
typedef enum QUP_RunStateCmdType
{
   QUP_RUNSTATE_CMD_RESET   = 0x2  , /**<Go to Reset State.          */
   QUP_RUNSTATE_CMD_RUN     = 0x1  , /**<Go to RUN state.            */
   QUP_RUNSTATE_CMD_PAUSE   = 0x3  , /**<Go to pause state.          */

   QUP_RUNSTATE_CMD_EXTEND  = 0x7FFFFFFF
} QUP_RunStateCmdType;

/** QUPe error state.                                                        */
typedef enum QUP_ErrStateType
{
   /**
      Indicates the output FIFO was full when a CRIF write was
      attempted to the FIFO. The write is discarded.
     */
   QUP_ERRSTATE_BAM_TRANS_ERROR  = 0x80, 

   /**
      Indicates the output FIFO was full when a CRIF write was
      attempted to the FIFO. The write is discarded.
     */
   QUP_ERRSTATE_OUTPUT_OVER_RUN  = 0x20,

   /**
      Indicates the input FIFO was empty when a CRIF read was
      attempted to the FIFO. The read value returned is
      indeterminate.
     */
   QUP_ERRSTATE_INPUT_UNDER_RUN  = 0x10,

   /**
      Indicates the output FIFO was empty when an output shift
      operation required a value.
     */
   QUP_ERRSTATE_OUTPUT_UNDER_RUN = 0x8,

   /**
      Indicates the input FIFO was full when an input shift
      operation provided a new value. When this happens, the
      new value is discarded.
     */
   QUP_ERRSTATE_INPUT_OVER_RUN   = 0x4,

   /**
      This is a field mask with all flags set.
     */
   QUP_ERRSTATE_FIELD = QUP_ERRSTATE_BAM_TRANS_ERROR  |
                         QUP_ERRSTATE_OUTPUT_OVER_RUN  |
                         QUP_ERRSTATE_INPUT_UNDER_RUN  |
                         QUP_ERRSTATE_OUTPUT_UNDER_RUN |
                         QUP_ERRSTATE_INPUT_OVER_RUN,

   QUP_ERRSTATE_EXTEND  = 0x7FFFFFFF
} QUP_ErrStateType;


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

/** Operation state.                                                         */
typedef enum I2C_ClkCtlType
{
   I2C_CLK_CTL_FS_DIVIDER_VALUE_MIN              = 0x7,
   I2C_CLK_CTL_FLAG_EXTEND  = 0x7FFFFFFF
} I2C_ClkCtlType;



/** Mini Cores List.                                                         */
typedef enum QUP_MiniCoreType
{
   QUP_MINI_CORE_FLD_MIN                  = 0  , /**<Smallest field value. */
   QUP_MINI_CORE_NULL                     = 0  , /**<NULL core.            */
   QUP_MINI_CORE_SPI                      = 1  , /**<SPI core.             */
   QUP_MINI_CORE_I2C_MASTER              = 0x2, /**<I2C master controller.*/
   QUP_MINI_CORE_FLD_MAX                  = 0x2, /**<Biggest field value.  */

   QUP_MINI_CORE_EXTEND           = 0x7FFFFFFF
} QUP_MiniCoreType;

#define QUP_CONFIG_N_8BITS                 7 // zero based



/** QUPe IO Modes.                                                           */
typedef enum QUP_IoModeType{
   QUP_IO_MODE_FLD_MIN = 0, /**<IO mode min value.           */
   QUP_IO_MODE_FIFO    = 0, /**<FIFO input-output mode.      */
   QUP_IO_MODE_BLOCK   = 1, /**<BLOCK input-output mode.     */
   QUP_IO_MODE_BAM     = 3, /**<DATA MOVER input-output mode.*/
   QUP_IO_MODE_FLD_MAX = 3, /**<IO mode max value.           */

   QUP_IO_MODE_EXTEND           = 0x7FFFFFFF
} QUP_IoModeType;




typedef enum QUP_I2cTagType
{
   QUP_I2C_TAGS_V1          = 0,    /**<I2C V1 TAGS.*/
   QUP_I2C_TAGS_V2          = 0x1, /**<I2C V2 TAGS.*/
   QUP_I2C_TAGS_FLD_MAX     = 0x1, /**<I2C V2 TAGS.*/
   QUP_I2C_TAGS_EXTEND           = 0x7FFFFFFF
} QUP_I2cTagType;

#endif /* __HAL_HALHWIOQUP_H__ */
