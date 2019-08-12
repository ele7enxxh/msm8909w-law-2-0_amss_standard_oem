#ifndef _HALQUPE_H
#define _HALQUPE_H

/*
===========================================================================

FILE:   HALqupe.h

DESCRIPTION:
    This file contains functions data structure declarations for QUP HAL

===========================================================================

        Edit History

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/buses/spi/src/hal/inc/HALqupe.h#1 $

When     Who    What, where, why
-------- ---    -----------------------------------------------------------
04/25/16 dpk    Added HS mode support
06/13/12 LK     Added I2C functions handling flush.
04/19/12 ddk    Added changes for avoiding symbol match with I2C library.
                Added updated HAL implementation used for I2C.
02/22/12 ddk    Added support for SPI BAM interface requirements.
08/26/09 LK     Created

===========================================================================
        Copyright c 2009-2016 Qualcomm Technologies Incorporated.
            All Rights Reserved.
            Qualcomm Proprietary/GTDR

===========================================================================
*/



#include "QupTypes.h"

#define HAL_QUPE_DATA_SEGMENT_LEN             255
#define HAL_QUPE_PACK28_I2C_STATE(MAS_STATUS, LINE_STATE)\
                ( ((MAS_STATUS) & 0x0FFFFFFC) |\
                  ((LINE_STATE) &0x03)         \
                )

//TODO: Ask HW designer for the values of the tags
typedef enum QUPI2CPD_TagsType {
   HAL_QUPE_TAG_START                        = 0x81,
   HAL_QUPE_TAG_START_10BIT                  = 0x89,
   HAL_QUPE_TAG_START_STOP                   = 0x8A,
   HAL_QUPE_TAG_DATA_WRITE                   = 0x82,
   HAL_QUPE_TAG_DATA_WRITE_STOP              = 0x83,
   HAL_QUPE_TAG_DATA_READ                    = 0x85,
   HAL_QUPE_TAG_DATA_READ_NACK               = 0x86,
   HAL_QUPE_TAG_DATA_READ_STOP               = 0x87,

   HAL_QUPE_TAG_NOP_WAIT                     = 0x80,
   HAL_QUPE_TAG_NOP_MARK                     = 0x90,
   HAL_QUPE_TAG_NOP_ID                       = 0x91,
   HAL_QUPE_TAG_TIME_STAMP                   = 0x92,
   HAL_QUPE_TAG_INPUT_EOT                    = 0x93,
   HAL_QUPE_TAG_INPUT_EOT_FLUSH              = 0x94,
   HAL_QUPE_TAG_NOP_LOCAL                    = 0x95,
   HAL_QUPE_TAG_FLUSH_STOP                   = 0x96,

   HAL_QUPE_DATA_TAG_EXTEND = 0x7FFFFFFF
} HAL_qupe_TagType;

/**  QUPe run states.                                                        */
typedef enum HAL_qupe_RunStateType
{
   /**
      Mini Core and related logic is held in reset state.
    */
   HAL_QUPE_RUNSTATE_RESET   = 0 ,

   /**
      Mini Core and related logic is released from reset and enters
      the run state.
    */
   HAL_QUPE_RUNSTATE_RUN     = 1 ,

   /**
      Pause state.
    */
   HAL_QUPE_RUNSTATE_PAUSE   = 3 ,

   /**
      State not valid.
    */
   HAL_QUPE_RUNSTATE_INVALID  = 0xFF ,

   HAL_QUPE_RUNSTATE_EXTEND  = 0x7FFFFFFF
} HAL_qupe_RunStateType;

/** Mini Cores List.                                                         */
typedef enum HAL_qupe_MiniCoreType
{
   HAL_QUPE_MINI_CORE_FLD_MIN                  = 0  , /**<Smallest field value. */
   HAL_QUPE_MINI_CORE_NULL                     = 0  , /**<NULL core.            */
   HAL_QUPE_MINI_CORE_SPI                      = 1  , /**<SPI core.             */
   HAL_QUPE_MINI_CORE_I2C_MASTER              = 0x2, /**<I2C master controller.*/
   HAL_QUPE_MINI_CORE_FLD_MAX                  = 0x2, /**<Biggest field value.  */

   HAL_QUPE_MINI_CORE_EXTEND           = 0x7FFFFFFF
} HAL_qupe_MiniCoreType;

typedef enum HAL_qupe_I2cTagType
{
   HAL_QUPE_I2C_TAGS_V1          = 0,    /**<I2C V1 TAGS.*/
   HAL_QUPE_I2C_TAGS_V2          = 0x1, /**<I2C V2 TAGS.*/
   HAL_QUPE_I2C_TAGS_FLD_MAX     = 0x1, /**<I2C V2 TAGS.*/
   HAL_QUPE_I2C_TAGS_EXTEND           = 0x7FFFFFFF
} HAL_qupe_I2cTagType;

/** QUPe IO Modes.                                                           */
typedef enum HAL_qupe_IOModeType{
   HAL_QUPE_IO_MODE_FLD_MIN = 0, /**<IO mode min value.           */
   HAL_QUPE_IO_MODE_FIFO    = 0, /**<FIFO input-output mode.      */
   HAL_QUPE_IO_MODE_BLOCK   = 1, /**<BLOCK input-output mode.     */
   HAL_QUPE_IO_MODE_BAM     = 3, /**<DATA MOVER input-output mode.*/
   HAL_QUPE_IO_MODE_FLD_MAX = 3, /**<IO mode max value.           */

   HAL_QUPE_IO_MODE_EXTEND           = 0x7FFFFFFF
} HAL_qupe_IOModeType;



/** The QUP operational state.                                               */
typedef enum HAL_qupe_OpStateType
{
   /**
      Notify When Done(NWD). Read only status bit which when set indicates
      a NWD acknowledgement is outstanding. This bit is set by QUP hw when
      NWD is signaled from BAM. This bit is cleared by QUP hw when NWD
      request is acknowledeg by assertion of done_toggle. This bit is
      always unconditionally cleared in RESET_STATE.
    */
   HAL_QUPE_OPSTATE_NWD             = 0x8000,
   /**
      Read only status bit which provides the current state
      of the side-band done_toggle signal sent to BAM. At
      each NWD acknowledgement, this bit toggles.
    */
   HAL_QUPE_OPSTATE_DONE_TOGGLE      = 0x4000,

   /**
            Read only. When set indicates that the input FIFO has
            BLOCK_SIZE_INPUT data ready to be read.
     */
   HAL_QUPE_OPSTATE_IN_BLOCK_READ_REQ     = 0x2000,

   /**
         Read only. When set indicates that the output FIFO needs
         BLOCK_SIZE_OUTPUT data to be written.
    */
   HAL_QUPE_OPSTATE_OUT_BLOCK_WRITE_REQ     = 0x1000,

   /**
      This flag is set by hardware whenever the programmed
      QUP_MX_INPUT_COUNT value is reached. This can be used
      in conjuction with the qup_input_service_irq assertion
      in Block_Mode and Data_Mover_Mode.
    */
   HAL_QUPE_OPSTATE_MAX_INPUT_DONE     = 0x800,

   /**
      This flag is set by whenever whenever the programmed
      QUP_MX_OUTPUT_COUNT value is reached. This can be used
      in conjuction with the qup_output_service_irq assertion in
      Block_Mode and Data_Mover_Mode.
    */
   HAL_QUPE_OPSTATE_MAX_OUTPUT_DONE    = 0x400,

   /**
      When set by hardware, indicates QUP Input FIFO has
      BLOCK_SIZE_INPUT amount of data ready for reading. At
      the point in time this bit is set, the hardware also asserts
      qup_input_service_irq. Valid only in Block_Mode.
     */
   HAL_QUPE_OPSTATE_INPUT_SERVICE      = 0x200,

   /**
      When set by hardware, indicates QUP Output FIFO needs
      BLOCK_SIZE_OUTPUT amount of data to be written. At
      the point in time this bit is set, the hardware also asserts
      qup_output_service_irq. Valid only in Block_Mode.
    */
   HAL_QUPE_OPSTATE_OUTPUT_SERVICE     = 0x100,

   /**
      When set, indicates the input FIFO is full and
      can accept no more data from the QUP Shift Register.
     Ready only.*/
   HAL_QUPE_OPSTATE_INPUT_FIFO_FULL      = 0x80,

   /**
      When set, indicates the output FIFO is full and
      can accept no more CRIF writes.
     Ready only.*/
   HAL_QUPE_OPSTATE_OUTPUT_FIFO_FULL     = 0x40,

   /**
      Read Only. When set, indicates the input FIFO has at least
      one value in it to be read. When clear, indicates the input
      FIFO is empty.Under FIFO_Mode, the
      qup_input_service_irq may be asserted when this bit
      changes from zero to one.
    */
   HAL_QUPE_OPSTATE_INPUT_FIFO_NOT_EMPTY = 0x20,

   /**
      Read Only. When set, indicates the output FIFO has at
      least one value in it to be shifted out. When clear, indicates
      the output FIFO is empty. Under FIFO_Mode, the
      qup_output_service_irq. is asserted when this bit
      changes from one to zero.
     */
   HAL_QUPE_OPSTATE_OUTPUT_FIFO_NOT_EMPTY = 0x10,

   /**
      This is a field mask with all flags set.
     */
   HAL_QUPE_OPSTATE_FIELD = HAL_QUPE_OPSTATE_NWD                  |
                            HAL_QUPE_OPSTATE_DONE_TOGGLE          |
                            HAL_QUPE_OPSTATE_MAX_INPUT_DONE       |
                            HAL_QUPE_OPSTATE_MAX_OUTPUT_DONE      |
                            HAL_QUPE_OPSTATE_INPUT_SERVICE        |
                            HAL_QUPE_OPSTATE_OUTPUT_SERVICE       |
                            HAL_QUPE_OPSTATE_INPUT_FIFO_FULL      |
                            HAL_QUPE_OPSTATE_OUTPUT_FIFO_FULL     |
                            HAL_QUPE_OPSTATE_INPUT_FIFO_NOT_EMPTY |
                            HAL_QUPE_OPSTATE_OUTPUT_FIFO_NOT_EMPTY,

   HAL_QUPE_OPSTATE_EXTEND  = 0x7FFFFFFF
} HAL_qupe_OpStateType;

/** QUPe error state.                                                        */
typedef enum HAL_qupe_ErrStateType
{
   /**
      Indicates the output FIFO was full when a CRIF write was
      attempted to the FIFO. The write is discarded.
     */
   HAL_QUPE_ERRSTATE_BAM_TRANS_ERROR  = 0x80, 

   /**
      Indicates the output FIFO was full when a CRIF write was
      attempted to the FIFO. The write is discarded.
     */
   HAL_QUPE_ERRSTATE_OUTPUT_OVER_RUN  = 0x20,

   /**
      Indicates the input FIFO was empty when a CRIF read was
      attempted to the FIFO. The read value returned is
      indeterminate.
     */
   HAL_QUPE_ERRSTATE_INPUT_UNDER_RUN  = 0x10,

   /**
      Indicates the output FIFO was empty when an output shift
      operation required a value.
     */
   HAL_QUPE_ERRSTATE_OUTPUT_UNDER_RUN = 0x8,

   /**
      Indicates the input FIFO was full when an input shift
      operation provided a new value. When this happens, the
      new value is discarded.
     */
   HAL_QUPE_ERRSTATE_INPUT_OVER_RUN   = 0x4,

   /**
      This is a field mask with all flags set.
     */
   HAL_QUPE_ERRSTATE_FIELD = HAL_QUPE_ERRSTATE_BAM_TRANS_ERROR  |
                             HAL_QUPE_ERRSTATE_OUTPUT_OVER_RUN  |
                             HAL_QUPE_ERRSTATE_INPUT_UNDER_RUN  |
                             HAL_QUPE_ERRSTATE_OUTPUT_UNDER_RUN |
                             HAL_QUPE_ERRSTATE_INPUT_OVER_RUN,

   HAL_QUPE_ERRSTATE_EXTEND  = 0x7FFFFFFF
} HAL_qupe_ErrStateType;

/** QUPe interrupt mask.                                                        */
typedef enum HAL_qupe_IrqMask
{
   /**
      If set SPI errors do not cause an interrupt.
     */
   HAL_QUPE_IRQMASK_SPI_ERRORS  = 0x1000000, 

   /**
      If set I2C errors do not cause an interrupt.
     */
   HAL_QUPE_IRQMASK_I2C_ERRORS  = 0x100000,
   
   /**
      INPUT_SERVICE_FLAG If set this flag in QUP_OPERATIONAL does not cause
      an interrupt.
     */
   HAL_QUPE_IRQMASK_INPUT_SERVICE_FLAG  = 0x200,

   /**
      OUTPUT_SERVICE_FLAG If set this flag in QUP_OPERATIONAL does not cause
      an interrupt.
     */
   HAL_QUPE_IRQMASK_OUTPUT_SERVICE_FLAG  = 0x100,

   /**
      This is a field mask with all flags set.
     */
   HAL_QUPE_IRQMASK_FIELD  = HAL_QUPE_IRQMASK_SPI_ERRORS  |
                             HAL_QUPE_IRQMASK_I2C_ERRORS  |
                             HAL_QUPE_IRQMASK_INPUT_SERVICE_FLAG |
                             HAL_QUPE_IRQMASK_OUTPUT_SERVICE_FLAG,

   HAL_QUPE_IRQMASK_EXTEND  = 0x7FFFFFFF
} HAL_qupe_IrqMask;

/* I2C Specific registers.                                                   */

/** I2C Input FSM states.                                                    */
typedef enum HAL_qupe_I2cInFsmStateType
{
   HAL_QUPE_I2C_IN_FSM_RESET_STATE          = 0,
   HAL_QUPE_I2C_IN_FSM_IDLE_STATE           = 1,
   HAL_QUPE_I2C_IN_FSM_READ_LAST_BYTE_STATE = 2,
   HAL_QUPE_I2C_IN_FSM_MIREC_STATE          = 3,
   HAL_QUPE_I2C_IN_FSM_DEC_STATE            = 4,
   HAL_QUPE_I2C_IN_FSM_STORE_STATE          = 5,

   HAL_QUPE_I2C_IN_FSM_STATE_EXTEND  = 0x7FFFFFFF
} HAL_qupe_I2cInFsmStateType;

/** I2C Output FSM state.                                                    */
typedef enum HAL_qupe_I2cOutFsmStateType
{
   HAL_QUPE_I2C_OUT_FSM_RESET_STATE          = 0,
   HAL_QUPE_I2C_OUT_FSM_IDLE_STATE           = 1,
   HAL_QUPE_I2C_OUT_FSM_DECODE_STATE         = 2,
   HAL_QUPE_I2C_OUT_FSM_SEND_STATE           = 3,
   HAL_QUPE_I2C_OUT_FSM_MIREC_STATE          = 4,
   HAL_QUPE_I2C_OUT_FSM_NOP_STATE            = 5,
   HAL_QUPE_I2C_OUT_FSM_INVALID_STATE        = 6,

   HAL_QUPE_I2C_OUT_FSM_STATE_EXTEND  = 0x7FFFFFFF
} HAL_qupe_I2cOutFsmStateType;

/** I2C Clock FSM state.                                                     */
typedef enum HAL_qupe_I2cClockStateType
{
   HAL_QUPE_I2C_CLOCK_RESET_BUSIDLE_STATE      = 0,
   HAL_QUPE_I2C_CLOCK_NOT_MASTER_STATE         = 1,
   HAL_QUPE_I2C_CLOCK_HIGH_STATE               = 2,
   HAL_QUPE_I2C_CLOCK_LOW_STATE                = 3,
   HAL_QUPE_I2C_CLOCK_HIGH_WAIT_STATE_STATE    = 4,
   HAL_QUPE_I2C_CLOCK_FORCED_LOW_STATE         = 5,
   HAL_QUPE_I2C_CLOCK_HS_ADDR_LOW_STATE        = 6,
   HAL_QUPE_I2C_CLOCK_DOUBLE_BUFFER_WAIT_STATE = 7,

   HAL_QUPE_I2C_CLOCK_STATE_EXTEND  = 0x7FFFFFFF
} HAL_qupe_I2cClockStateType;

/** I2C Data FSM state.                                                      */
typedef enum HAL_qupe_I2cDataStateType
{
   HAL_QUPE_I2C_DATA_RESET_WAIT_STATE        = 0,
   HAL_QUPE_I2C_DATA_TX_ADDR_STATE           = 1,
   HAL_QUPE_I2C_DATA_TX_DATA_STATE           = 2,
   HAL_QUPE_I2C_DATA_TX_HS_ADDR_STATE        = 3,
   HAL_QUPE_I2C_DATA_TX_10_BIT_ADDR_STATE    = 4,
   HAL_QUPE_I2C_DATA_TX_2ND_BYTE_STATE       = 5,
   HAL_QUPE_I2C_DATA_RX_DATA_STATE           = 6,

   HAL_QUPE_I2C_DATA_STATE_EXTEND  = 0x7FFFFFFF
} HAL_qupe_I2cDataStateType;

/** Operation state.                                                         */
typedef enum HAL_qupe_I2cOperationFailedFlagsType
{
   HAL_QUPE_I2C_OP_STATUS_INVALID_READ_SEQ    = 0x2000000,
   HAL_QUPE_I2C_OP_STATUS_INVALID_READ_ADDR   = 0x1000000,
   HAL_QUPE_I2C_OP_STATUS_INVALID_TAG         = 0x800000,
   HAL_QUPE_I2C_OP_STATUS_FAILED              = 0xC0,
   HAL_QUPE_I2C_OP_STATUS_INVALID_WRITE       = 0x20,
   HAL_QUPE_I2C_OP_STATUS_ARB_LOST            = 0x10,
   HAL_QUPE_I2C_OP_STATUS_PACKET_NACKED       = 0x08,
   HAL_QUPE_I2C_OP_STATUS_BUS_ERROR           = 0x04,

   HAL_QUPE_I2C_OP_STATUS_ERROR_FIELD =
                  HAL_QUPE_I2C_OP_STATUS_INVALID_READ_SEQ |
                  HAL_QUPE_I2C_OP_STATUS_INVALID_READ_ADDR|
                  HAL_QUPE_I2C_OP_STATUS_INVALID_TAG      |
                  HAL_QUPE_I2C_OP_STATUS_FAILED           |
                  HAL_QUPE_I2C_OP_STATUS_INVALID_WRITE    |
                  HAL_QUPE_I2C_OP_STATUS_ARB_LOST         |
                  HAL_QUPE_I2C_OP_STATUS_PACKET_NACKED    |
                  HAL_QUPE_I2C_OP_STATUS_BUS_ERROR        ,

   HAL_QUPE_I2C_OP_STATUS_FLAG_EXTEND  = 0x7FFFFFFF
} HAL_qupe_I2cOperationFailedFlagsType;


typedef struct HAL_qupe_I2cStatus
{
   uint32                        uRawStatus;
   uint32                        uRawLineState;
} HAL_qupe_I2cStatus;

//TODO: Check if this expanded state is needed?
typedef struct HAL_qupe_ExtendedI2cStatus
{
   HAL_qupe_I2cInFsmStateType    eInFsmState;    /**<I2C Input FSM state.    */
   HAL_qupe_I2cOutFsmStateType   eOutFsmState;   /**<I2C Output FSM state.   */
   HAL_qupe_I2cClockStateType    eClockState;    /**<I2C Clock FSM state.    */
   HAL_qupe_I2cDataStateType     eDataState;     /**I2C Data FSM state.      */
   /**
       I2C Operation state.
       The uOpStateFlags value is an or of the enum
       HAL_qupe_I2cOperationStateFlagsType.
     */
   uint32                        uOpStateFlags;
   uint32                        uRawStatus;
   uint32                        uRawLineState;
} HAL_qupe_ExtendedI2cStatus;

/** @enum HAL_qupe_ResultType QUPe HAL result values.      */
typedef enum HAL_qupe_ResultType{
   HAL_QUPE_RESULT_SUCCESS = 0, /**< Sucessful operation.             */
   /** Generic errors. */
   HAL_QUPE_RESULT_INVALID_ARG, /**< ERROR INVALID ARGUMENT.          */
   HAL_QUPE_RESULT_NOT_SUPORTED_ARG, /**< ERROR NOT SUPPORTED.        */
   HAL_QUPE_RESULT_OUT_FIFO_FULL,    /**< ERROR OUTPUT FIFO IS FULL.  */
   HAL_QUPE_RESULT_IN_FIFO_EMPTY,    /**< ERROR INPUT FIFO IS EMPTY.  */
   HAL_QUPE_RESULT_INVALID_FS_DIVIDER, /**< ERROR FS DIVIDER TOO SMALL.  */

   HAL_QUPE_RESULT_EXTEND  = 0x7FFFFFFF
} HAL_qupe_ResultType;

/**
 * This type specifies whether the core is the master or slave
 * on the SPI bus.
 */
typedef enum
{
   HAL_QUPE_SPI_MASTER, /**< the SPI core is the master */
   HAL_QUPE_SPI_SLAVE /**< the SPI core is the slave */
} HAL_QUPE_SPI_CoreModeType;

/**
 * This type specifies the clock polarity the core should use to
 * communicate with a device.
 */
typedef enum {
   HAL_QUPE_SPI_CLK_IDLE_LOW, /**< the SPI clock is low when idle. first clock edge is rising. */
   HAL_QUPE_SPI_CLK_IDLE_HIGH /**< the SPI clock is high when idle. first clock edge is falling. */
} HAL_QUPE_SPI_ClockPolarityType;

/**
 * This type specifies the clock phase mode that the SPI core
 * will should use to communicate with a device.
 */
typedef enum {
   HAL_QUPE_SPI_INPUT_FIRST_MODE, /**< the SPI data input signal is sampled on the leading clock edge */ 
   HAL_QUPE_SPI_OUTPUT_FIRST_MODE /**< the SPI data input signal is be sampled on the trailing clock edge */
} HAL_QUPE_SPI_TransferModeType;

/**
 * When the SPI core is the master, this type is used to specify
 * whether the core should configure the chip select output to
 * be active high or active low.
 * 
 * When in slave mode, the core always expects an active low
 * chip select input.
 */
typedef enum {
   HAL_QUPE_SPI_CS_ACTIVE_LOW, /**< the chip select is active low */
   HAL_QUPE_SPI_CS_ACTIVE_HIGH  /**< the chip select is active high */
} HAL_QUPE_SPI_ChipSelectPolarityType;
/**
 * This type specifies whether the core should operate in
 * loopback mode.
 *
 * When loopback mode is enabled, the SPI core internally ties
 * SPI Data Out to SPI Data In. This mode is intended for
 * testing and debugging.
 */
typedef enum {
   HAL_QUPE_SPI_HS_DISABLED, /**< Disable high performance mode */
   HAL_QUPE_SPI_HS_ENABLED   /**< Enable high performance mode */
} HAL_QUPE_SPI_HS;


/**
 * This type specifies whether the core should operate in
 * loopback mode.
 *
 * When loopback mode is enabled, the SPI core internally ties
 * SPI Data Out to SPI Data In. This mode is intended for
 * testing and debugging.
 */
typedef enum {
   HAL_QUPE_SPI_LOOPBACK_DISABLED, /*< Disable internal loopback mode */
   HAL_QUPE_SPI_LOOPBACK_ENABLED /*< Enable internal loopback mode */
} HAL_QUPE_SPI_LoopbackModeType;

/**
 * This type specifies whether the SPI core should use so-called
 * Input Only, Output Only, or Bi-directional modes during the
 * transfer.
 * 
 * In Input Only mode, the SPI Data Out signal is tied low
 * during the transfer and no Output-related interrupts are
 * triggered during the transfer.
 * 
 * Similarly, in Output Only, the SPI Data In signal is ignored
 * by the SPI core and no Input-related interrupts are triggered
 * during the transfer.
 * 
 * In Bi-directional mode, data is both shifted in and out as
 * normal in a SPI full-duplex configuration.
 */
typedef enum {
   HAL_QUPE_SPI_INPUT_ONLY,
   HAL_QUPE_SPI_OUTPUT_ONLY,
   HAL_QUPE_SPI_BI_DIRECTIONAL
} HAL_QUPE_SPI_TransferDirectionType;



typedef struct HAL_qupe_HwInfoType
{

   uint32                uNumInputSlots;
   uint32                uNumOutputSlots;
   uint32                uNumOutputBlockSlots;
   uint32                uNumOutputBlocks;
   uint32                uNumInputBlockSlots;
   uint32                uNumInputBlocks;
   uint32                uHWVersion;
} HAL_qupe_HwInfoType;

/**
 * This type is used with HAL_QUPE_SPI_SetDeviceParameters to
 * configure the device parameters to be used by the SPI core
 * for the current transfer.
 */
typedef struct {
   HAL_QUPE_SPI_ClockPolarityType eClockPolarity; /**< the SPI clock polarity */
   HAL_QUPE_SPI_TransferModeType eTransferMode; /**< the SPI clock phase */
   boolean bClockAlwaysOn; /**< set to TRUE to run the SPI clock continuously from start to end of the transfer */
   boolean bChipSelectAlwaysOn; /**< set to TRUE*/
   HAL_QUPE_SPI_ChipSelectPolarityType eCSPolarity; /**< the chip-select polarity */
   uint32 nChipSelectId; /**< the chip select to use for this transfer */
   uint32 nDeassertionClockTicks; /**< the number of clock ticks to wait between transfer units */
   HAL_QUPE_SPI_CoreModeType   eSpiCoreMode;
   HAL_QUPE_SPI_HS             eSpiCoreHS;
} HAL_QUPE_SPI_DeviceParametersType;

/**
 * This type is used with HAL_QUPE_SPI_TransferParameters to
 * configure the transfer parameters to be used by the SPI core
 * for the current transfer
 */
typedef struct {
   uint8 nNumBits; /**< the number of bits per transfer unit */
   HAL_QUPE_SPI_LoopbackModeType eLoopbackMode; /**< configures internal loopback mode */
   boolean bReceivedDataPacked; /**< set TRUE if data is to be packed in the SPI input FIFO */
   boolean bTransmitDataPacked; /**< set to TRUE if data is packed in the SPI output FIFO */
   boolean bOutputBitShift; /**< set to TRUE to enable the "Output Bit Shift" function */
   uint32 nNumInputTransfers; /**< the number of transfer units that will be read into the input FIFO */
   uint32 nNumOutputTransfers; /**< the number of transfer units that will be sent from the output FIFO */
   HAL_QUPE_SPI_TransferDirectionType eTransferDirection; /**< configures the direction of the transfer */
   HAL_qupe_IOModeType eIOMode; /**< specifies whether to use FIFO, block, or data mover to transfer data */
} HAL_QUPE_SPI_TransferParametersType; 

/**
 * This type is used for creation of command descriptors
 * to be used for SPI BAM mode operation.
 */
typedef struct
{
   uint32 cmd_element_base;
   uint32 cmd_element_index;
   uint32 cmd_size_in_bytes;
} HAL_qupe_BamCommandDescType;


/** @brief This function initializes the qupe HAL.
    @param[in]  pQupAddr qup block address.
    @param[in]  pBlockAddress Memeory address of the qup block. 
    @param[in]  ppszVersion   Version pointer.
    @return Nothing.
  */
void HAL_qupe_Init
(
  uint8 *pQupAddr,
  char **ppszVersion
);



/** @brief Configures the mini core.
    @param[in]  pQupAddr qup block address.
    @return Nothing.
  */
void HAL_qupe_Configure
(
   uint8 *pQupAddr
);


/** @brief Reads the RTL info and returns it.
    @param[in]  pQupAddr qup block address.
    @param[in]  pHwInfo Pointer to struct used to store hw info.
    @return Nothing.
  */
void HAL_qupe_GetHwInfo
(
   uint8 *pQupAddr          ,
   HAL_qupe_HwInfoType *pHwInfo
);

void
HAL_qupe_SetInputFifoEnabled
(
   uint8 *pQupAddr,
   const boolean bInFifoEnabled
);


/** @brief Gets the state of the input fifo.
    @param[in]  pQupAddr qup block address.
    @return TRUE if the input fifo is enabled, FALSE otherwise.
  */
boolean
HAL_qupe_GetInputFifoEnabled
(
   uint8 *pQupAddr
);


/** @brief Enables/disables the output fifo.
    @param[in]  pQupAddr qup block address.
    @param[in]  bOutFifoEnabled Boolean indicating whether to enable the
                output fifo.
    @return Nothing.
  */
void
HAL_qupe_SetOutputFifoEnabled
(
   uint8 *pQupAddr,
   const boolean bOutFifoEnabled
);


/** @brief Gets the state of the output fifo enabled.
    @param[in]  pQupAddr qup block address.
    @return TRUE if the output fifo is enabled, FALSE otherwise.
  */
boolean
HAL_qupe_GetOutputFifoEnabled
(
   uint8 *pQupAddr
);


/** @brief Gets the state of the run state field.
    @param[in]  pQupAddr qup block address.
    @return TRUE if the run state is valid, FALSE otherwise.
  */
boolean
HAL_qupe_RunStateIsValid
(
   uint8 *pQupAddr
);

/** @brief Sets the state of the QUPe.
    @param[in]  pQupAddr qup block address.
    @param[in]  ceQupeRunState Specifies the new state.
    @return HAL_QUPE_RESULT_SUCCESS if successful, error code otherwise.
  */
HAL_qupe_ResultType
HAL_qupe_SetRunState
(
   uint8 *pQupAddr,
   const HAL_qupe_RunStateType ceQupeRunState
);


/** @brief Gets the run state of the QUPe.
    @param[in]  pQupAddr qup block address.
    @return QUPe run state.
  */
HAL_qupe_RunStateType
HAL_qupe_GetRunState
(
   uint8 *pQupAddr
);


/** @brief Sets the output and input mode to FIFO, Block or BAM Mode.
    @param[in]  pQupAddr qup block address.
    @param[in]  ceOutMode  output mode FIFO, BLOCK or BAM mode.
    @param[in]  ceInpMode  input mode FIFO, BLOCK or BAM mode.
    @return     None.
    @note based on RTL setting only FIFO Mode may be available.
    @sa HAL_qupe_GetOutInMode
  */
/*
void
HAL_qupe_SetOutInMode
(
   uint8 *pQupAddr                ,
   const HAL_qupe_IOModeType ceOutMode,
   const HAL_qupe_IOModeType ceInpMode
);
*/

/** @brief Sets the output mode to FIFO, Block or BAM Mode.
    @param[in]  pQupAddr qup block address.
    @param[in]  ceOutMode  output mode FIFO, BLOCK or BAM mode.
    @return     None.
    @note based on RTL setting only FIFO Mode may be available.
    @sa HAL_qupe_GetOutInMode
  */
void
HAL_qupe_SetOutIoMode
(
   uint8 *pQupAddr                ,
   const HAL_qupe_IOModeType ceOutMode
);

/** @brief Sets the input mode to FIFO, Block or BAM Mode.
    @param[in]  pQupAddr qup block address.
    @param[in]  ceInpMode  input mode FIFO, BLOCK or BAM mode.
    @return     None.
    @note based on RTL setting only FIFO Mode may be available.
    @sa HAL_qupe_GetOutInMode
  */
void
HAL_qupe_SetInIoMode
(
   uint8 *pQupAddr                ,
   const HAL_qupe_IOModeType ceInpMode
);

/** @brief Get's the output and input mode.
    @param[in]  pQupAddr qup block address.
    @param[in]  pOutMode pointer to store output mode.
    @param[in]  pInpMode pointer to store input mode.
    @return     Nothing.
    @sa HAL_qupe_SetOutInMode
  */
void
HAL_qupe_GetOutInMode
(
   uint8 *pQupAddr,
   HAL_qupe_IOModeType *pOutMode,
   HAL_qupe_IOModeType *pInpMode
);


/** @brief Resets QUPe.
    @param[in]  pQupAddr qup block address.
    @return Nothing.
  */
void
HAL_qupe_Reset
(
   uint8   *pQupAddr
);


/** @brief Clears the specified operational flags.
    @param[in]  pQupAddr qup block address.
    @param[in]  uOpFlagsBitmap  bitmap of HAL_qupe_OpStateType
                flags to be cleared.
    @return HAL_QUPE_RESULT_SUCCESS if successful, error code otherwise.
  */
HAL_qupe_ResultType
HAL_qupe_ClearOpStatusFlags
(
   uint8         *pQupAddr,
   const uint32  uOpFlagsBitmap
);

/** @brief Gets the operational flags.
    @param[in]  pQupAddr qup block address.
    @return bitmap with operational flags from
            HAL_qupe_OpStateType set.
  */
uint32
HAL_qupe_GetOpStatusFlags
(
   uint8   *pQupAddr
);


/** @brief Clears the specified operational flags.
    @param[in]  pQupAddr qup block address.
    @param[in]  uErrFlagsBitmap bitmap of error flags to be cleared.
    @return HAL_QUPE_RESULT_SUCCESS if successful, error code otherwise.
  */
HAL_qupe_ResultType
HAL_qupe_ClearErrStatusFlags
(
   uint8         *pQupAddr,
   const uint32  uErrFlagsBitmap
);

/** @brief Gets the error flags.
    @param[in]  pQupAddr qup block address.
    @return bitmap with error flags set.
  */
uint32
HAL_qupe_GetErrStatusFlags
(
   uint8   *pQupAddr
);


/** @brief Sets the enabled error flags list.
          If set, each flag enables generation of qup_error_irq and the
          setting of the corresponding error flag in the QUP_ERROR_FLAGS
          register for the specified error case.
    @param[in]  pQupAddr qup block address.
    @param[in]  uEnabledErrFlagsBm bitmap of error flags to enable.
    @return  HAL_QUPE_RESULT_SUCCESS if successful, error code otherwise.
    @sa HAL_qupe_GetEnabledErrStatusFlags
  */
HAL_qupe_ResultType
HAL_qupe_SetErrStatusIrqEnabled
(
   uint8         *pQupAddr,
   const uint32  uEnabledErrFlagsBm
);


/** @brief Get the list of enabled error flags.
          If set, each flag enables generation of qup_error_irq and the
          setting of the corresponding error flag in the QUP_ERROR_FLAGS
          register for the specified error case.
    @param[in]  pQupAddr qup block address.
    @return bitmap with error flags set.
    @sa HAL_qupe_SetEnabledErrStatusFlags
  */
uint32
HAL_qupe_GetErrStatusIrqEnabled
(
   uint8   *pQupAddr
);




/** @brief Gets the current number of writes of size N to the
          Mini Core. A value of zero indicates the output
          count function is not enabled for use.
    @param[in]  pQupAddr qup block address.
    @return the number of writes.
    @note HAL_qupe_SetOutIoMode must have been called before to set the mode.
  */
uint32
HAL_qupe_GetOutTransferCountCurrent
(
   uint8   *pQupAddr
);

/** @brief Gets the Number of slots filled in the output FIFO.
    @param[in]  pQupAddr qup block address.
    @return Number of slots filled in the output FIFO.
  */
uint32
HAL_qupe_GetOutFifoOccupiedSlotsCount
(
   uint8   *pQupAddr
);


/** @brief Puts a word into output fifo.
    @param[in]  pQupAddr qup block address.
    @param[in]  uOutWord the word to be put in output fifo.
    @return HAL_QUPE_RESULT_SUCCESS if successful, error code otherwise.
  */
HAL_qupe_ResultType
HAL_qupe_PutWordIntoOutFifo
(
   uint8        *pQupAddr,
   const uint32 uOutWord
);


/** @brief Gets the number of reads of size N from the Mini Core per
           RUN_STATE. A value of zero indicates the input count
           function is not enabled for use.
    @param[in]  pQupAddr qup block address.
    @return the max number of reads.
    @note HAL_qupe_SetInIoMode must have been called before to set the mode.
  */
uint32
HAL_qupe_GetInTransferCountCurrent
(
   uint8   *pQupAddr
);

/** @brief Sets the interrupt mask.
          If set, each flag masks generation of interrupt..
    @param[in]  pQupAddr Qup Block address.
    @param[in]  uIrqBm bitmap from HAL_qupe_IrqMask of interrupts
                to be masked.
    @return  Nothing.
  */
void HAL_qupe_SetIrqMask
(
   uint8         *pQupAddr,
   const uint32  uIrqBm
);

/** @brief Sets the number of Writes size N.
          This is used only if the core is in FIFO_Mode.
    @param[in]  pQupAddr qup block address.
    @param[in]  uCount the number of writes.
    @return HAL_QUPE_RESULT_SUCCESS if successful, error code otherwise.
    @note HAL_qupe_SetOutIoMode must have been called before to set the mode.
  */
HAL_qupe_ResultType
HAL_qupe_SetOutTransferCount
(
   uint8         *pQupAddr,
   const uint32  uCount
);


/** @brief Sets the number of Reads of size N.
    @param[in]  pQupAddr qup block address.
    @param[in]  uTransferCount the max number of reads.
    @return the max number of reads.
    @note HAL_qupe_SetInIoMode must have been called before to set the mode.
  */
HAL_qupe_ResultType
HAL_qupe_SetInTransferCount
(
   uint8   *pQupAdd,
   const uint32  uTransferCount
);


/** @brief Gets the Number of filled slots in the input FIFO.
    @param[in]  pQupAddr qup block address.
    @return Number of filled slots in the input FIFO.
  */
uint32
HAL_qupe_GetInFifoOccupiedSlotsCount
(
   uint8  *pQupAddr
);


/** @brief Gets a word from input fifo.
    @param[in]  pQupAddr qup block address.
    @param[out] pOutWord pointer to the word from output fifo.
    @return HAL_QUPE_RESULT_SUCCESS if successful, error code otherwise.
  */
HAL_qupe_ResultType
HAL_qupe_GetWordFromInFifo
(
   uint8   *pQupAddr,
   uint32  *pInWord
);


/** @brief Sets the fast standard clock given the input clock freq in KHz.
    @param[in]  pQupAddr qup block address.
    @param[in]  eI2cTags       Tag version.
    @param[in]  uInputClockKhz Input clock to the i2c core in KHz.
    @param[in]  uHSI2cBusClockReqKhz Requested clock in KHz.
                I2C freq in KHz.	
   
    @return HAL_QUPE_RESULT_SUCCESS if successful, error code otherwise.
  */
HAL_qupe_ResultType
HAL_qupe_SetI2cConfig
(
   uint8                      *pQupAddr,
   const HAL_qupe_I2cTagType  eI2cTags,
   const uint32               uInputClockKhz,
   const uint32               uFSI2cBusClockReqKhz
);


/** @brief Issues bus clear cmd that is 9 clock cycles.
    @param[in]  pQupAddr qup block address.
    @return Nothing.
  */
void
HAL_qupe_ClearI2CBus
(
   uint8  *pQupAddr
);

/** @brief Checks if bus is clear.
    @param[in]  pQupAddr qup block address.
    @return Nothing.
  */
boolean
HAL_qupe_IsI2CBusBusy
(
  uint8                      *pQupAddr
);

/** @brief Gets the state of the I2C.
    @param[in]  pQupAddr qup block address.
    @param[out] pI2cStatus pointer to a location to store status struct.
    @return     Nothing.
  */
void
HAL_qupe_GetI2cState
(
   uint8                *pQupAddr,
   HAL_qupe_I2cStatus   *pI2cStatus
);

/** @brief Gets the state of the I2C.
    @param[in]  pQupAddr qup block address.
    @param[out] uErrMask errors HAL_qupe_I2cOperationFailedFlagsType.
    @return     Nothing.
  */
void
HAL_qupe_ClearI2cErrors
(
   uint8               *pQupAddr,
   uint32              uErrMask
);

/** @brief Packs the operational state for debug.
    @param[in]  pQupAddr qup block address.
    @return     28 bit value indicating state.
  */
uint32 HAL_qupe_Pack28_OperationalState
(
   uint8                *pQupAddr
);

/** @brief Flushes the i2c up to the FLASH STOP.
    @param[in]  pQupAddr qup block address.
  */
HAL_qupe_ResultType
HAL_qupe_I2cFlush
(
   uint8 *pQupAddr
);

/** @brief returns the state of the flash cmd.
    @param[in]  pQupAddr qup block address.
    @return     TRUE if flush complete FALSE otherwise.
  */
boolean
HAL_qupe_IsI2cFlushComplete
(
   uint8 *pQupAddr
);

/**
 * This function configures the specified QUP Mini core selection.
 * 
 * @param[in]  pQupAddr Qup Block address.
 * @param eMiniCoreType - The mini core type.
 */
void HAL_qupe_SetSelectedMinicore
(
   uint8  *pQupAddr, 
   HAL_qupe_MiniCoreType eMiniCoreType
);


/**
 * This function configures the SPI device-specific parameters
 * before starting an SPI transfer.
 * 
 * @param[in]  pQupAddr Qup Block address.
 * @param pmDeviceParameters - Pointer to the HAL_QUPE_SPI_DeviceConfiguration
 *                          struct which contains the device configuration
 *                          parameters.
 */
void HAL_qupe_Spi_SetDeviceParameters(uint8  *pQupAddr, 
  HAL_QUPE_SPI_DeviceParametersType *pmDeviceParameters);


/**
 * This function configures the transfer-specific parameters
 * before starting an SPI transfer.
 * 
 * @param[in]  pQupAddr Qup Block address.
 * @param pmTransferParameters - Pointer to the
 *                  HAL_QUPE_SPI_TransferParametersType struct which
 *                  contains the transfer parameters.
 */
void HAL_qupe_Spi_SetTransferParameters
(
   uint8  *pQupAddr, 
   HAL_QUPE_SPI_TransferParametersType *pmTransferParameters
);

/**
 * This function configures the specified SPI core to run in
 * master or slave mode.
 * 
 * @param[in]  pQupAddr Qup Block address.
 * @param eCoreMode - The core mode.
 */
void HAL_qupe_Spi_SetCoreMode
(
   uint8  *pQupAddr, 
   HAL_QUPE_SPI_CoreModeType eCoreMode
);

/**
 * This function shall perform command descriptor creation for 
 * given set of transfer parameters.
 * 
 * @param[in]  pQupAddr Qup Block address.
 * @param mHalTransferParameters - The pointer to the transfer parameters.
 * @param halBamCmdDescInfo      - The pointer to the command descriptor details.
 *
 */
int32
HAL_qupe_Spi_SetTransferParameters_CommandElement
(
   uint8         *pQupAddr,
   HAL_QUPE_SPI_TransferParametersType *mHalTransferParameters,
   HAL_qupe_BamCommandDescType *halBamCmdDescInfo
);



/**
 * This function shall perform command descriptor creation for 
 * given set of device parameters.
 * 
 * @param[in]  uPhysicalQupAddr     - Physical Qup Block address. 
 * @param[in]  uVirtualQupAddr       - Virtual Qup Block address. 
 * @param mHalDeviceParameters   - The pointer to the device parameters.
 * @param halBamCmdDescInfo      - The pointer to the command descriptor details.
 *
 */
int32
HAL_qupe_Spi_SetDeviceParameters_CommandElement
(
   uint8         *uPhysicalQupAddr,
   uint8         *uVirtualQupAddr,
   HAL_QUPE_SPI_DeviceParametersType *mHalDeviceParameters,
   HAL_qupe_BamCommandDescType *halBamCmdDescInfo
);


/**
 * This function shall perform command descriptor creation for 
 * setting Minicore state.
 * 
 * @param[in]  pQupAddr Qup Block address.
 * @param eMiniCoreMode          - The minicore type to be set.
 * @param halBamCmdDescInfo      - The pointer to the command descriptor details.
 *
 */
int32
HAL_qupe_SetCoreMode_CommandElement
(
   uint8         *pQupAddr,
   HAL_qupe_MiniCoreType eMiniCoreMode,
   HAL_qupe_BamCommandDescType *halBamCmdDescInfo
);


/**
 * This function shall perform command descriptor creation for 
 * setting Minicore state.
 * 
 * @param[in]  pQupAddr Qup Block address.
 * @param ceQupeRunState         - The QUP core state to be set.
 * @param halBamCmdDescInfo      - The pointer to the command descriptor details.
 *
 */
int32
HAL_qupe_SetCoreState_CommandElement
(
   uint8         *pQupAddr,
   HAL_qupe_RunStateType ceQupeRunState,
   HAL_qupe_BamCommandDescType *halBamCmdDescInfo
);

/**
 * This function configures QUP interrupts in MSS by
 * enabling an TCSR register
 * 
 * @param[in]  pTcsrAddr TCR address.
 * @param[in]  bitMask - Bit to be set for a QUP.
 * @param[in]  bitShift - value to be set for a QUP.
 */
int32 
HAL_qupe_EnableTcsrInterrupt
(
 uint8 *pTcsrAddr,
 uint32 bitMask,
 uint32 bitShift
 );

#endif /* _HALQUPE_H */


