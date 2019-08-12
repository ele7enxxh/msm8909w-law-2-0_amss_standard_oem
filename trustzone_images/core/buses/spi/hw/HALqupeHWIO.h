#ifndef __HAL_HALHWIOQUP_H__
#define __HAL_HALHWIOQUP_H__

/* ===========================================================================

FILE:   HALqupeHWIO.h

DESCRIPTION:
    This file contains register offset as well as registe field offsets
    and widths definitions.

==============================================================================

        Edit History

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/buses/spi/hw/HALqupeHWIO.h#1 $

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
#define HWIO_INTLOCK()
//#define INTLOCK()

#define HWIO_INTFREE()
//#define INTFREE()


#define __inp(port)         (*((volatile unsigned char *) (port)))
#define __inpw(port)        (*((volatile unsigned short *) (port)))
#define __inpdw(port)       (*((volatile unsigned int *) (port)))

#define __outp(port, val)   (*((volatile unsigned char *) (port)) = ((unsigned char) (val)))
#define __outpw(port, val)  (*((volatile unsigned short *) (port)) = ((unsigned short) (val)))
#define __outpdw(port, val) (*((volatile unsigned int *) (port)) = ((unsigned int) (val)))


#define in_byte(addr)               (__inp(addr))
#define in_byte_masked(addr, mask)  (__inp(addr) & (mask)) 
#define out_byte(addr, val)         __outp(addr,val)
#define out_byte_masked(io, mask, val, shadow)  \
  HWIO_INTLOCK();    \
  (void) out_byte( io, shadow); \
  shadow = (shadow & (unsigned short)(~(mask))) | ((unsigned short)((val) & (mask))); \
  HWIO_INTFREE()
#define out_byte_masked_ns(io, mask, val, current_reg_content)  \
  (void) out_byte( io, ((current_reg_content & (unsigned short)(~(mask))) | \
                       ((unsigned short)((val) & (mask)))) )

#define in_word(addr)              (__inpw(addr))
#define in_word_masked(addr, mask) (__inpw(addr) & (mask))
#define out_word(addr, val)        __outpw(addr,val)
#define out_word_masked(io, mask, val, shadow)  \
  HWIO_INTLOCK( ); \
  shadow = (shadow & (unsigned short)(~(mask))) |  ((unsigned short)((val) & (mask))); \
  (void) out_word( io, shadow); \
  HWIO_INTFREE( )
#define out_word_masked_ns(io, mask, val, current_reg_content)  \
  (void) out_word( io, ((current_reg_content & (unsigned short)(~(mask))) | \
                       ((unsigned short)((val) & (mask)))) )

#define in_dword(addr)              (__inpdw(addr))
#define in_dword_masked(addr, mask) (__inpdw(addr) & (mask))
#define out_dword(addr, val)        __outpdw(addr,val)
#define out_dword_masked(io, mask, val, shadow)  \
   HWIO_INTLOCK( ); \
   shadow = (shadow & (unsigned int)(~(mask))) | ((unsigned int)((val) & (mask))); \
   (void) out_dword( io, shadow); \
   HWIO_INTFREE( )
#define out_dword_masked_ns(io, mask, val, current_reg_content) \
  (void) out_dword( io, ((current_reg_content & (unsigned int)(~(mask))) | ((unsigned int)((val) & (mask)))) )

#define HWIO_GSBI_QUP_CONFIG_OFFSET                                                                     (0x0000)
#define HWIO_GSBI_QUP_CONFIG_RMSK                                                                        0xf3fdf
#define HWIO_GSBI_QUP_CONFIG_IN(GSBI_QUP_ADDR)\
   in_dword_masked((GSBI_QUP_ADDR)+HWIO_GSBI_QUP_CONFIG_OFFSET, HWIO_GSBI_QUP_CONFIG_RMSK)
#define HWIO_GSBI_QUP_CONFIG_INM(GSBI_QUP_ADDR,m)\
   in_dword_masked((GSBI_QUP_ADDR)+HWIO_GSBI_QUP_CONFIG_OFFSET, m)
#define HWIO_GSBI_QUP_CONFIG_OUT(GSBI_QUP_ADDR,v)\
   out_dword((GSBI_QUP_ADDR)+HWIO_GSBI_QUP_CONFIG_OFFSET, v)
#define HWIO_GSBI_QUP_CONFIG_OUTM(GSBI_QUP_ADDR,m,v)\
   out_dword_masked_ns((GSBI_QUP_ADDR)+HWIO_GSBI_QUP_CONFIG_OFFSET, m,v,HWIO_GSBI_QUP_CONFIG_IN(GSBI_QUP_ADDR))
#define HWIO_GSBI_QUP_CONFIG_EN_EXT_STATE_VALID_BMSK                                                     0x80000
#define HWIO_GSBI_QUP_CONFIG_EN_EXT_STATE_VALID_SHFT                                                        0x13
#define HWIO_GSBI_QUP_CONFIG_EN_EXT_BLOCK_RD_BMSK                                                        0x40000
#define HWIO_GSBI_QUP_CONFIG_EN_EXT_BLOCK_RD_SHFT                                                           0x12
#define HWIO_GSBI_QUP_CONFIG_EN_EXT_BLOCK_WRT_BMSK                                                       0x20000
#define HWIO_GSBI_QUP_CONFIG_EN_EXT_BLOCK_WRT_SHFT                                                          0x11
#define HWIO_GSBI_QUP_CONFIG_EN_EXT_OUT_FLAG_BMSK                                                        0x10000
#define HWIO_GSBI_QUP_CONFIG_EN_EXT_OUT_FLAG_SHFT                                                           0x10
#define HWIO_GSBI_QUP_CONFIG_CORE_CLK_ON_EN_BMSK                                                          0x2000
#define HWIO_GSBI_QUP_CONFIG_CORE_CLK_ON_EN_SHFT                                                             0xd
#define HWIO_GSBI_QUP_CONFIG_APP_CLK_ON_EN_BMSK                                                           0x1000
#define HWIO_GSBI_QUP_CONFIG_APP_CLK_ON_EN_SHFT                                                              0xc
#define HWIO_GSBI_QUP_CONFIG_MINI_CORE_BMSK                                                                0xf00
#define HWIO_GSBI_QUP_CONFIG_MINI_CORE_SHFT                                                                  0x8
#define HWIO_GSBI_QUP_CONFIG_NO_INPUT_BMSK                                                                  0x80
#define HWIO_GSBI_QUP_CONFIG_NO_INPUT_SHFT                                                                   0x7
#define HWIO_GSBI_QUP_CONFIG_NO_OUTPUT_BMSK                                                                 0x40
#define HWIO_GSBI_QUP_CONFIG_NO_OUTPUT_SHFT                                                                  0x6
#define HWIO_GSBI_QUP_CONFIG_N_BMSK                                                                         0x1f
#define HWIO_GSBI_QUP_CONFIG_N_SHFT                                                                          0x0

#define HWIO_GSBI_QUP_STATE_OFFSET                                                                      (0x0004)
#define HWIO_GSBI_QUP_STATE_RMSK                                                                            0x1f
#define HWIO_GSBI_QUP_STATE_IN(GSBI_QUP_ADDR)\
   in_dword_masked((GSBI_QUP_ADDR)+HWIO_GSBI_QUP_STATE_OFFSET, HWIO_GSBI_QUP_STATE_RMSK)
#define HWIO_GSBI_QUP_STATE_INM(GSBI_QUP_ADDR,m)\
   in_dword_masked((GSBI_QUP_ADDR)+HWIO_GSBI_QUP_STATE_OFFSET, m)
#define HWIO_GSBI_QUP_STATE_OUT(GSBI_QUP_ADDR,v)\
   out_dword((GSBI_QUP_ADDR)+HWIO_GSBI_QUP_STATE_OFFSET, v)
#define HWIO_GSBI_QUP_STATE_OUTM(GSBI_QUP_ADDR,m,v)\
   out_dword_masked_ns((GSBI_QUP_ADDR)+HWIO_GSBI_QUP_STATE_OFFSET, m,v,HWIO_GSBI_QUP_STATE_IN(GSBI_QUP_ADDR))
#define HWIO_GSBI_QUP_STATE_I2C_FLUSH_BMSK                                                                  0x40
#define HWIO_GSBI_QUP_STATE_I2C_FLUSH_SHFT                                                                   0x6
#define HWIO_GSBI_QUP_STATE_I2C_MAST_GEN_BMSK                                                               0x10
#define HWIO_GSBI_QUP_STATE_I2C_MAST_GEN_SHFT                                                                0x4
#define HWIO_GSBI_QUP_STATE_SPI_GEN_BMSK                                                                     0x8
#define HWIO_GSBI_QUP_STATE_SPI_GEN_SHFT                                                                     0x3
#define HWIO_GSBI_QUP_STATE_STATE_VALID_BMSK                                                                 0x4
#define HWIO_GSBI_QUP_STATE_STATE_VALID_SHFT                                                                 0x2
#define HWIO_GSBI_QUP_STATE_STATE_BMSK                                                                       0x3
#define HWIO_GSBI_QUP_STATE_STATE_SHFT                                                                       0x0

#define HWIO_GSBI_QUP_IO_MODES_OFFSET                                                                   (0x0008)
#define HWIO_GSBI_QUP_IO_MODES_RMSK                                                                      0x1ffff
#define HWIO_GSBI_QUP_IO_MODES_IN(GSBI_QUP_ADDR)\
   in_dword_masked((GSBI_QUP_ADDR)+HWIO_GSBI_QUP_IO_MODES_OFFSET, HWIO_GSBI_QUP_IO_MODES_RMSK)
#define HWIO_GSBI_QUP_IO_MODES_INM(GSBI_QUP_ADDR,m)\
   in_dword_masked((GSBI_QUP_ADDR)+HWIO_GSBI_QUP_IO_MODES_OFFSET, m)
#define HWIO_GSBI_QUP_IO_MODES_OUT(GSBI_QUP_ADDR,v)\
   out_dword((GSBI_QUP_ADDR)+HWIO_GSBI_QUP_IO_MODES_OFFSET, v)
#define HWIO_GSBI_QUP_IO_MODES_OUTM(GSBI_QUP_ADDR,m,v)\
   out_dword_masked_ns((GSBI_QUP_ADDR)+HWIO_GSBI_QUP_IO_MODES_OFFSET, m,v,HWIO_GSBI_QUP_IO_MODES_IN(GSBI_QUP_ADDR))
#define HWIO_GSBI_QUP_IO_MODES_OUTPUT_BIT_SHIFT_EN_BMSK                                                  0x10000
#define HWIO_GSBI_QUP_IO_MODES_OUTPUT_BIT_SHIFT_EN_SHFT                                                     0x10
#define HWIO_GSBI_QUP_IO_MODES_PACK_EN_BMSK                                                               0x8000
#define HWIO_GSBI_QUP_IO_MODES_PACK_EN_SHFT                                                                  0xf
#define HWIO_GSBI_QUP_IO_MODES_UNPACK_EN_BMSK                                                             0x4000
#define HWIO_GSBI_QUP_IO_MODES_UNPACK_EN_SHFT                                                                0xe
#define HWIO_GSBI_QUP_IO_MODES_INPUT_MODE_BMSK                                                            0x3000
#define HWIO_GSBI_QUP_IO_MODES_INPUT_MODE_SHFT                                                               0xc
#define HWIO_GSBI_QUP_IO_MODES_OUTPUT_MODE_BMSK                                                            0xc00
#define HWIO_GSBI_QUP_IO_MODES_OUTPUT_MODE_SHFT                                                              0xa
#define HWIO_GSBI_QUP_IO_MODES_INPUT_FIFO_SIZE_BMSK                                                        0x380
#define HWIO_GSBI_QUP_IO_MODES_INPUT_FIFO_SIZE_SHFT                                                          0x7
#define HWIO_GSBI_QUP_IO_MODES_INPUT_BLOCK_SIZE_BMSK                                                        0x60
#define HWIO_GSBI_QUP_IO_MODES_INPUT_BLOCK_SIZE_SHFT                                                         0x5
#define HWIO_GSBI_QUP_IO_MODES_OUTPUT_FIFO_SIZE_BMSK                                                        0x1c
#define HWIO_GSBI_QUP_IO_MODES_OUTPUT_FIFO_SIZE_SHFT                                                         0x2
#define HWIO_GSBI_QUP_IO_MODES_OUTPUT_BLOCK_SIZE_BMSK                                                        0x3
#define HWIO_GSBI_QUP_IO_MODES_OUTPUT_BLOCK_SIZE_SHFT                                                        0x0

#define HWIO_GSBI_QUP_SW_RESET_OFFSET                                                                   (0x000C)
#define HWIO_GSBI_QUP_SW_RESET_RMSK                                                                   0xffffffff
#define HWIO_GSBI_QUP_SW_RESET_OUT(GSBI_QUP_ADDR,v)\
   out_dword((GSBI_QUP_ADDR)+HWIO_GSBI_QUP_SW_RESET_OFFSET, v)
#define HWIO_GSBI_QUP_SW_RESET_OUTM(GSBI_QUP_ADDR,m,v)\
   out_dword_masked_ns((GSBI_QUP_ADDR)+HWIO_GSBI_QUP_SW_RESET_OFFSET, m,v,HWIO_GSBI_QUP_SW_RESET_IN(GSBI_QUP_ADDR))
#define HWIO_GSBI_QUP_SW_RESET_QUP_SW_RESET_BMSK                                                      0xffffffff
#define HWIO_GSBI_QUP_SW_RESET_QUP_SW_RESET_SHFT                                                             0x0

#define HWIO_GSBI_QUP_TIME_OUT_OFFSET                                                                   (0x0010)
#define HWIO_GSBI_QUP_TIME_OUT_RMSK                                                                   0xffffffff
#define HWIO_GSBI_QUP_TIME_OUT_IN(GSBI_QUP_ADDR)\
   in_dword_masked((GSBI_QUP_ADDR)+HWIO_GSBI_QUP_TIME_OUT_OFFSET, HWIO_GSBI_QUP_TIME_OUT_RMSK)
#define HWIO_GSBI_QUP_TIME_OUT_INM(GSBI_QUP_ADDR,m)\
   in_dword_masked((GSBI_QUP_ADDR)+HWIO_GSBI_QUP_TIME_OUT_OFFSET, m)
#define HWIO_GSBI_QUP_TIME_OUT_OUT(GSBI_QUP_ADDR,v)\
   out_dword((GSBI_QUP_ADDR)+HWIO_GSBI_QUP_TIME_OUT_OFFSET, v)
#define HWIO_GSBI_QUP_TIME_OUT_OUTM(GSBI_QUP_ADDR,m,v)\
   out_dword_masked_ns((GSBI_QUP_ADDR)+HWIO_GSBI_QUP_TIME_OUT_OFFSET, m,v,HWIO_GSBI_QUP_TIME_OUT_IN(GSBI_QUP_ADDR))
#define HWIO_GSBI_QUP_TIME_OUT_NOP_BMSK                                                               0xffff0000
#define HWIO_GSBI_QUP_TIME_OUT_NOP_SHFT                                                                     0x10
#define HWIO_GSBI_QUP_TIME_OUT_TIME_OUT_VALUE_BMSK                                                        0xffff
#define HWIO_GSBI_QUP_TIME_OUT_TIME_OUT_VALUE_SHFT                                                           0x0

#define HWIO_GSBI_QUP_TIME_OUT_CURRENT_OFFSET                                                           (0x0014)
#define HWIO_GSBI_QUP_TIME_OUT_CURRENT_RMSK                                                               0xffff
#define HWIO_GSBI_QUP_TIME_OUT_CURRENT_IN(GSBI_QUP_ADDR)\
   in_dword_masked((GSBI_QUP_ADDR)+HWIO_GSBI_QUP_TIME_OUT_CURRENT_OFFSET, HWIO_GSBI_QUP_TIME_OUT_CURRENT_RMSK)
#define HWIO_GSBI_QUP_TIME_OUT_CURRENT_INM(GSBI_QUP_ADDR,m)\
   in_dword_masked((GSBI_QUP_ADDR)+HWIO_GSBI_QUP_TIME_OUT_CURRENT_OFFSET, m)
#define HWIO_GSBI_QUP_TIME_OUT_CURRENT_TIME_OUT_CURRENT_BMSK                                              0xffff
#define HWIO_GSBI_QUP_TIME_OUT_CURRENT_TIME_OUT_CURRENT_SHFT                                                 0x0

#define HWIO_GSBI_QUP_OPERATIONAL_OFFSET                                                                (0x0018)
#define HWIO_GSBI_QUP_OPERATIONAL_RMSK                                                                    0xfff0
#define HWIO_GSBI_QUP_OPERATIONAL_IN(GSBI_QUP_ADDR)\
   in_dword_masked((GSBI_QUP_ADDR)+HWIO_GSBI_QUP_OPERATIONAL_OFFSET, HWIO_GSBI_QUP_OPERATIONAL_RMSK)
#define HWIO_GSBI_QUP_OPERATIONAL_INM(GSBI_QUP_ADDR,m)\
   in_dword_masked((GSBI_QUP_ADDR)+HWIO_GSBI_QUP_OPERATIONAL_OFFSET, m)
#define HWIO_GSBI_QUP_OPERATIONAL_OUT(GSBI_QUP_ADDR,v)\
   out_dword((GSBI_QUP_ADDR)+HWIO_GSBI_QUP_OPERATIONAL_OFFSET, v)
#define HWIO_GSBI_QUP_OPERATIONAL_OUTM(GSBI_QUP_ADDR,m,v)\
   out_dword_masked_ns((GSBI_QUP_ADDR)+HWIO_GSBI_QUP_OPERATIONAL_OFFSET, m,v,HWIO_GSBI_QUP_OPERATIONAL_IN(GSBI_QUP_ADDR))
#define HWIO_GSBI_QUP_OPERATIONAL_NWD_BMSK                                                                0x8000
#define HWIO_GSBI_QUP_OPERATIONAL_NWD_SHFT                                                                   0xf
#define HWIO_GSBI_QUP_OPERATIONAL_DONE_TOGGLE_BMSK                                                        0x4000
#define HWIO_GSBI_QUP_OPERATIONAL_DONE_TOGGLE_SHFT                                                           0xe
#define HWIO_GSBI_QUP_OPERATIONAL_IN_BLOCK_READ_REQ_BMSK                                                  0x2000
#define HWIO_GSBI_QUP_OPERATIONAL_IN_BLOCK_READ_REQ_SHFT                                                     0xd
#define HWIO_GSBI_QUP_OPERATIONAL_OUT_BLOCK_WRITE_REQ_BMSK                                                0x1000
#define HWIO_GSBI_QUP_OPERATIONAL_OUT_BLOCK_WRITE_REQ_SHFT                                                   0xc
#define HWIO_GSBI_QUP_OPERATIONAL_MAX_INPUT_DONE_FLAG_BMSK                                                 0x800
#define HWIO_GSBI_QUP_OPERATIONAL_MAX_INPUT_DONE_FLAG_SHFT                                                   0xb
#define HWIO_GSBI_QUP_OPERATIONAL_MAX_OUTPUT_DONE_FLAG_BMSK                                                0x400
#define HWIO_GSBI_QUP_OPERATIONAL_MAX_OUTPUT_DONE_FLAG_SHFT                                                  0xa
#define HWIO_GSBI_QUP_OPERATIONAL_INPUT_SERVICE_FLAG_BMSK                                                  0x200
#define HWIO_GSBI_QUP_OPERATIONAL_INPUT_SERVICE_FLAG_SHFT                                                    0x9
#define HWIO_GSBI_QUP_OPERATIONAL_OUTPUT_SERVICE_FLAG_BMSK                                                 0x100
#define HWIO_GSBI_QUP_OPERATIONAL_OUTPUT_SERVICE_FLAG_SHFT                                                   0x8
#define HWIO_GSBI_QUP_OPERATIONAL_INPUT_FIFO_FULL_BMSK                                                      0x80
#define HWIO_GSBI_QUP_OPERATIONAL_INPUT_FIFO_FULL_SHFT                                                       0x7
#define HWIO_GSBI_QUP_OPERATIONAL_OUTPUT_FIFO_FULL_BMSK                                                     0x40
#define HWIO_GSBI_QUP_OPERATIONAL_OUTPUT_FIFO_FULL_SHFT                                                      0x6
#define HWIO_GSBI_QUP_OPERATIONAL_INPUT_FIFO_NOT_EMPTY_BMSK                                                 0x20
#define HWIO_GSBI_QUP_OPERATIONAL_INPUT_FIFO_NOT_EMPTY_SHFT                                                  0x5
#define HWIO_GSBI_QUP_OPERATIONAL_OUTPUT_FIFO_NOT_EMPTY_BMSK                                                0x10
#define HWIO_GSBI_QUP_OPERATIONAL_OUTPUT_FIFO_NOT_EMPTY_SHFT                                                 0x4

#define HWIO_GSBI_QUP_ERROR_FLAGS_OFFSET                                                                (0x001C)
#define HWIO_GSBI_QUP_ERROR_FLAGS_RMSK                                                                      0x7c
#define HWIO_GSBI_QUP_ERROR_FLAGS_IN(GSBI_QUP_ADDR)\
   in_dword_masked((GSBI_QUP_ADDR)+HWIO_GSBI_QUP_ERROR_FLAGS_OFFSET, HWIO_GSBI_QUP_ERROR_FLAGS_RMSK)
#define HWIO_GSBI_QUP_ERROR_FLAGS_INM(GSBI_QUP_ADDR,m)\
   in_dword_masked((GSBI_QUP_ADDR)+HWIO_GSBI_QUP_ERROR_FLAGS_OFFSET, m)
#define HWIO_GSBI_QUP_ERROR_FLAGS_OUT(GSBI_QUP_ADDR,v)\
   out_dword((GSBI_QUP_ADDR)+HWIO_GSBI_QUP_ERROR_FLAGS_OFFSET, v)
#define HWIO_GSBI_QUP_ERROR_FLAGS_OUTM(GSBI_QUP_ADDR,m,v)\
   out_dword_masked_ns((GSBI_QUP_ADDR)+HWIO_GSBI_QUP_ERROR_FLAGS_OFFSET, m,v,HWIO_GSBI_QUP_ERROR_FLAGS_IN(GSBI_QUP_ADDR))
#define HWIO_GSBI_QUP_ERROR_FLAGS_TIME_OUT_ERR_BMSK                                                         0x40
#define HWIO_GSBI_QUP_ERROR_FLAGS_TIME_OUT_ERR_SHFT                                                          0x6
#define HWIO_GSBI_QUP_ERROR_FLAGS_OUTPUT_OVER_RUN_ERR_BMSK                                                  0x20
#define HWIO_GSBI_QUP_ERROR_FLAGS_OUTPUT_OVER_RUN_ERR_SHFT                                                   0x5
#define HWIO_GSBI_QUP_ERROR_FLAGS_INPUT_UNDER_RUN_ERR_BMSK                                                  0x10
#define HWIO_GSBI_QUP_ERROR_FLAGS_INPUT_UNDER_RUN_ERR_SHFT                                                   0x4
#define HWIO_GSBI_QUP_ERROR_FLAGS_OUTPUT_UNDER_RUN_ERR_BMSK                                                  0x8
#define HWIO_GSBI_QUP_ERROR_FLAGS_OUTPUT_UNDER_RUN_ERR_SHFT                                                  0x3
#define HWIO_GSBI_QUP_ERROR_FLAGS_INPUT_OVER_RUN_ERR_BMSK                                                    0x4
#define HWIO_GSBI_QUP_ERROR_FLAGS_INPUT_OVER_RUN_ERR_SHFT                                                    0x2

#define HWIO_GSBI_QUP_ERROR_FLAGS_EN_OFFSET                                                             (0x0020)
#define HWIO_GSBI_QUP_ERROR_FLAGS_EN_RMSK                                                                   0x7c
#define HWIO_GSBI_QUP_ERROR_FLAGS_EN_IN(GSBI_QUP_ADDR)\
   in_dword_masked((GSBI_QUP_ADDR)+HWIO_GSBI_QUP_ERROR_FLAGS_EN_OFFSET, HWIO_GSBI_QUP_ERROR_FLAGS_EN_RMSK)
#define HWIO_GSBI_QUP_ERROR_FLAGS_EN_INM(GSBI_QUP_ADDR,m)\
   in_dword_masked((GSBI_QUP_ADDR)+HWIO_GSBI_QUP_ERROR_FLAGS_EN_OFFSET, m)
#define HWIO_GSBI_QUP_ERROR_FLAGS_EN_OUT(GSBI_QUP_ADDR,v)\
   out_dword((GSBI_QUP_ADDR)+HWIO_GSBI_QUP_ERROR_FLAGS_EN_OFFSET, v)
#define HWIO_GSBI_QUP_ERROR_FLAGS_EN_OUTM(GSBI_QUP_ADDR,m,v)\
   out_dword_masked_ns((GSBI_QUP_ADDR)+HWIO_GSBI_QUP_ERROR_FLAGS_EN_OFFSET, m,v,HWIO_GSBI_QUP_ERROR_FLAGS_EN_IN(GSBI_QUP_ADDR))
#define HWIO_GSBI_QUP_ERROR_FLAGS_EN_TIME_OUT_ERR_EN_BMSK                                                   0x40
#define HWIO_GSBI_QUP_ERROR_FLAGS_EN_TIME_OUT_ERR_EN_SHFT                                                    0x6
#define HWIO_GSBI_QUP_ERROR_FLAGS_EN_OUTPUT_OVER_RUN_ERR_EN_BMSK                                            0x20
#define HWIO_GSBI_QUP_ERROR_FLAGS_EN_OUTPUT_OVER_RUN_ERR_EN_SHFT                                             0x5
#define HWIO_GSBI_QUP_ERROR_FLAGS_EN_INPUT_UNDER_RUN_ERR_EN_BMSK                                            0x10
#define HWIO_GSBI_QUP_ERROR_FLAGS_EN_INPUT_UNDER_RUN_ERR_EN_SHFT                                             0x4
#define HWIO_GSBI_QUP_ERROR_FLAGS_EN_OUTPUT_UNDER_RUN_ERR_EN_BMSK                                            0x8
#define HWIO_GSBI_QUP_ERROR_FLAGS_EN_OUTPUT_UNDER_RUN_ERR_EN_SHFT                                            0x3
#define HWIO_GSBI_QUP_ERROR_FLAGS_EN_INPUT_OVER_RUN_ERR_EN_BMSK                                              0x4
#define HWIO_GSBI_QUP_ERROR_FLAGS_EN_INPUT_OVER_RUN_ERR_EN_SHFT                                              0x2

#define HWIO_GSBI_QUP_TEST_CTRL_OFFSET                                                                  (0x0024)
#define HWIO_GSBI_QUP_TEST_CTRL_RMSK                                                                         0x1
#define HWIO_GSBI_QUP_TEST_CTRL_IN(GSBI_QUP_ADDR)\
   in_dword_masked((GSBI_QUP_ADDR)+HWIO_GSBI_QUP_TEST_CTRL_OFFSET, HWIO_GSBI_QUP_TEST_CTRL_RMSK)
#define HWIO_GSBI_QUP_TEST_CTRL_INM(GSBI_QUP_ADDR,m)\
   in_dword_masked((GSBI_QUP_ADDR)+HWIO_GSBI_QUP_TEST_CTRL_OFFSET, m)
#define HWIO_GSBI_QUP_TEST_CTRL_OUT(GSBI_QUP_ADDR,v)\
   out_dword((GSBI_QUP_ADDR)+HWIO_GSBI_QUP_TEST_CTRL_OFFSET, v)
#define HWIO_GSBI_QUP_TEST_CTRL_OUTM(GSBI_QUP_ADDR,m,v)\
   out_dword_masked_ns((GSBI_QUP_ADDR)+HWIO_GSBI_QUP_TEST_CTRL_OFFSET, m,v,HWIO_GSBI_QUP_TEST_CTRL_IN(GSBI_QUP_ADDR))
#define HWIO_GSBI_QUP_TEST_CTRL_QUP_TEST_BUS_EN_BMSK                                                         0x1
#define HWIO_GSBI_QUP_TEST_CTRL_QUP_TEST_BUS_EN_SHFT                                                         0x0

#define HWIO_GSBI_QUP_OPERATIONAL_MASK_OFFSET                                                           (0x0028)
#define HWIO_GSBI_QUP_OPERATIONAL_MASK_RMSK                                                                0x300
#define HWIO_GSBI_QUP_OPERATIONAL_MASK_IN(GSBI_QUP_ADDR)\
   in_dword_masked((GSBI_QUP_ADDR)+HWIO_GSBI_QUP_OPERATIONAL_MASK_OFFSET, HWIO_GSBI_QUP_OPERATIONAL_MASK_RMSK)
#define HWIO_GSBI_QUP_OPERATIONAL_MASK_INM(GSBI_QUP_ADDR,m)\
   in_dword_masked((GSBI_QUP_ADDR)+HWIO_GSBI_QUP_OPERATIONAL_MASK_OFFSET, m)
#define HWIO_GSBI_QUP_OPERATIONAL_MASK_OUT(GSBI_QUP_ADDR,v)\
   out_dword((GSBI_QUP_ADDR)+HWIO_GSBI_QUP_OPERATIONAL_MASK_OFFSET, v)
#define HWIO_GSBI_QUP_OPERATIONAL_MASK_OUTM(GSBI_QUP_ADDR,m,v)\
   out_dword_masked_ns((GSBI_QUP_ADDR)+HWIO_GSBI_QUP_OPERATIONAL_MASK_OFFSET, m,v,HWIO_GSBI_QUP_OPERATIONAL_MASK_IN(GSBI_QUP_ADDR))
#define HWIO_GSBI_QUP_OPERATIONAL_MASK_INPUT_SERVICE_MASK_BMSK                                             0x200
#define HWIO_GSBI_QUP_OPERATIONAL_MASK_INPUT_SERVICE_MASK_SHFT                                               0x9
#define HWIO_GSBI_QUP_OPERATIONAL_MASK_OUTPUT_SERVICE_MASK_BMSK                                            0x100
#define HWIO_GSBI_QUP_OPERATIONAL_MASK_OUTPUT_SERVICE_MASK_SHFT                                              0x8

#define HWIO_GSBI_QUP_HW_VERSION_OFFSET                                                                 (0x0030)
#define HWIO_GSBI_QUP_HW_VERSION_RMSK                                                                 0xffffffff
#define HWIO_GSBI_QUP_HW_VERSION_IN(GSBI_QUP_ADDR)\
   in_dword_masked((GSBI_QUP_ADDR)+HWIO_GSBI_QUP_HW_VERSION_OFFSET, HWIO_GSBI_QUP_HW_VERSION_RMSK)
#define HWIO_GSBI_QUP_HW_VERSION_INM(GSBI_QUP_ADDR,m)\
   in_dword_masked((GSBI_QUP_ADDR)+HWIO_GSBI_QUP_HW_VERSION_OFFSET, m)
#define HWIO_GSBI_QUP_HW_VERSION_QUP_HW_VERSION_BMSK                                                  0xffffffff
#define HWIO_GSBI_QUP_HW_VERSION_QUP_HW_VERSION_SHFT                                                         0x0

#define HWIO_GSBI_QUP_MX_OUTPUT_COUNT_OFFSET                                                            (0x0100)
#define HWIO_GSBI_QUP_MX_OUTPUT_COUNT_RMSK                                                                0xffff
#define HWIO_GSBI_QUP_MX_OUTPUT_COUNT_IN(GSBI_QUP_ADDR)\
   in_dword_masked((GSBI_QUP_ADDR)+HWIO_GSBI_QUP_MX_OUTPUT_COUNT_OFFSET, HWIO_GSBI_QUP_MX_OUTPUT_COUNT_RMSK)
#define HWIO_GSBI_QUP_MX_OUTPUT_COUNT_INM(GSBI_QUP_ADDR,m)\
   in_dword_masked((GSBI_QUP_ADDR)+HWIO_GSBI_QUP_MX_OUTPUT_COUNT_OFFSET, m)
#define HWIO_GSBI_QUP_MX_OUTPUT_COUNT_OUT(GSBI_QUP_ADDR,v)\
   out_dword((GSBI_QUP_ADDR)+HWIO_GSBI_QUP_MX_OUTPUT_COUNT_OFFSET, v)
#define HWIO_GSBI_QUP_MX_OUTPUT_COUNT_OUTM(GSBI_QUP_ADDR,m,v)\
   out_dword_masked_ns((GSBI_QUP_ADDR)+HWIO_GSBI_QUP_MX_OUTPUT_COUNT_OFFSET, m,v,HWIO_GSBI_QUP_MX_OUTPUT_COUNT_IN(GSBI_QUP_ADDR))
#define HWIO_GSBI_QUP_MX_OUTPUT_COUNT_MX_OUTPUT_COUNT_BMSK                                                0xffff
#define HWIO_GSBI_QUP_MX_OUTPUT_COUNT_MX_OUTPUT_COUNT_SHFT                                                   0x0

#define HWIO_GSBI_QUP_MX_OUTPUT_CNT_CURRENT_OFFSET                                                      (0x0104)
#define HWIO_GSBI_QUP_MX_OUTPUT_CNT_CURRENT_RMSK                                                          0xffff
#define HWIO_GSBI_QUP_MX_OUTPUT_CNT_CURRENT_IN(GSBI_QUP_ADDR)\
   in_dword_masked((GSBI_QUP_ADDR)+HWIO_GSBI_QUP_MX_OUTPUT_CNT_CURRENT_OFFSET, HWIO_GSBI_QUP_MX_OUTPUT_CNT_CURRENT_RMSK)
#define HWIO_GSBI_QUP_MX_OUTPUT_CNT_CURRENT_INM(GSBI_QUP_ADDR,m)\
   in_dword_masked((GSBI_QUP_ADDR)+HWIO_GSBI_QUP_MX_OUTPUT_CNT_CURRENT_OFFSET, m)
#define HWIO_GSBI_QUP_MX_OUTPUT_CNT_CURRENT_MX_OUTPUT_CNT_CURRENT_BMSK                                    0xffff
#define HWIO_GSBI_QUP_MX_OUTPUT_CNT_CURRENT_MX_OUTPUT_CNT_CURRENT_SHFT                                       0x0

#define HWIO_GSBI_QUP_OUTPUT_DEBUG_OFFSET                                                               (0x0108)
#define HWIO_GSBI_QUP_OUTPUT_DEBUG_RMSK                                                               0xffffffff
#define HWIO_GSBI_QUP_OUTPUT_DEBUG_IN(GSBI_QUP_ADDR)\
   in_dword_masked((GSBI_QUP_ADDR)+HWIO_GSBI_QUP_OUTPUT_DEBUG_OFFSET, HWIO_GSBI_QUP_OUTPUT_DEBUG_RMSK)
#define HWIO_GSBI_QUP_OUTPUT_DEBUG_INM(GSBI_QUP_ADDR,m)\
   in_dword_masked((GSBI_QUP_ADDR)+HWIO_GSBI_QUP_OUTPUT_DEBUG_OFFSET, m)
#define HWIO_GSBI_QUP_OUTPUT_DEBUG_OUTPUT_DEBUG_DATA_BMSK                                             0xffffffff
#define HWIO_GSBI_QUP_OUTPUT_DEBUG_OUTPUT_DEBUG_DATA_SHFT                                                    0x0

#define HWIO_GSBI_QUP_OUTPUT_FIFO_WORD_CNT_OFFSET                                                       (0x010C)
#define HWIO_GSBI_QUP_OUTPUT_FIFO_WORD_CNT_RMSK                                                            0x1ff
#define HWIO_GSBI_QUP_OUTPUT_FIFO_WORD_CNT_IN(GSBI_QUP_ADDR)\
   in_dword_masked((GSBI_QUP_ADDR)+HWIO_GSBI_QUP_OUTPUT_FIFO_WORD_CNT_OFFSET, HWIO_GSBI_QUP_OUTPUT_FIFO_WORD_CNT_RMSK)
#define HWIO_GSBI_QUP_OUTPUT_FIFO_WORD_CNT_INM(GSBI_QUP_ADDR,m)\
   in_dword_masked((GSBI_QUP_ADDR)+HWIO_GSBI_QUP_OUTPUT_FIFO_WORD_CNT_OFFSET, m)
#define HWIO_GSBI_QUP_OUTPUT_FIFO_WORD_CNT_OUTPUT_FIFO_WORD_CNT_BMSK                                       0x1ff
#define HWIO_GSBI_QUP_OUTPUT_FIFO_WORD_CNT_OUTPUT_FIFO_WORD_CNT_SHFT                                         0x0

#define HWIO_GSBI_QUP_OUTPUT_FIFO0_OFFSET                                                               (0x0110)
#define HWIO_GSBI_QUP_OUTPUT_FIFO0_RMSK                                                               0xffffffff
#define HWIO_GSBI_QUP_OUTPUT_FIFO0_OUT(GSBI_QUP_ADDR,v)\
   out_dword((GSBI_QUP_ADDR)+HWIO_GSBI_QUP_OUTPUT_FIFO0_OFFSET, v)
#define HWIO_GSBI_QUP_OUTPUT_FIFO0_OUTM(GSBI_QUP_ADDR,m,v)\
   out_dword_masked_ns((GSBI_QUP_ADDR)+HWIO_GSBI_QUP_OUTPUT_FIFO0_OFFSET, m,v,HWIO_GSBI_QUP_OUTPUT_FIFO0_IN(GSBI_QUP_ADDR))
#define HWIO_GSBI_QUP_OUTPUT_FIFO0_OUTPUT_BMSK                                                        0xffffffff
#define HWIO_GSBI_QUP_OUTPUT_FIFO0_OUTPUT_SHFT                                                               0x0

#define HWIO_GSBI_QUP_MX_WRITE_COUNT_OFFSET                                                             (0x0150)
#define HWIO_GSBI_QUP_MX_WRITE_COUNT_RMSK                                                                 0xffff
#define HWIO_GSBI_QUP_MX_WRITE_COUNT_IN(GSBI_QUP_ADDR)\
   in_dword_masked((GSBI_QUP_ADDR)+HWIO_GSBI_QUP_MX_WRITE_COUNT_OFFSET, HWIO_GSBI_QUP_MX_WRITE_COUNT_RMSK)
#define HWIO_GSBI_QUP_MX_WRITE_COUNT_INM(GSBI_QUP_ADDR,m)\
   in_dword_masked((GSBI_QUP_ADDR)+HWIO_GSBI_QUP_MX_WRITE_COUNT_OFFSET, m)
#define HWIO_GSBI_QUP_MX_WRITE_COUNT_OUT(GSBI_QUP_ADDR,v)\
   out_dword((GSBI_QUP_ADDR)+HWIO_GSBI_QUP_MX_WRITE_COUNT_OFFSET, v)
#define HWIO_GSBI_QUP_MX_WRITE_COUNT_OUTM(GSBI_QUP_ADDR,m,v)\
   out_dword_masked_ns((GSBI_QUP_ADDR)+HWIO_GSBI_QUP_MX_WRITE_COUNT_OFFSET, m,v,HWIO_GSBI_QUP_MX_WRITE_COUNT_IN(GSBI_QUP_ADDR))
#define HWIO_GSBI_QUP_MX_WRITE_COUNT_MX_WRITE_COUNT_BMSK                                                  0xffff
#define HWIO_GSBI_QUP_MX_WRITE_COUNT_MX_WRITE_COUNT_SHFT                                                     0x0

#define HWIO_GSBI_QUP_MX_WRITE_CNT_CURRENT_OFFSET                                                       (0x0154)
#define HWIO_GSBI_QUP_MX_WRITE_CNT_CURRENT_RMSK                                                           0xffff
#define HWIO_GSBI_QUP_MX_WRITE_CNT_CURRENT_IN(GSBI_QUP_ADDR)\
   in_dword_masked((GSBI_QUP_ADDR)+HWIO_GSBI_QUP_MX_WRITE_CNT_CURRENT_OFFSET, HWIO_GSBI_QUP_MX_WRITE_CNT_CURRENT_RMSK)
#define HWIO_GSBI_QUP_MX_WRITE_CNT_CURRENT_INM(GSBI_QUP_ADDR,m)\
   in_dword_masked((GSBI_QUP_ADDR)+HWIO_GSBI_QUP_MX_WRITE_CNT_CURRENT_OFFSET, m)
#define HWIO_GSBI_QUP_MX_WRITE_CNT_CURRENT_MX_WRITE_CNT_CURRENT_BMSK                                      0xffff
#define HWIO_GSBI_QUP_MX_WRITE_CNT_CURRENT_MX_WRITE_CNT_CURRENT_SHFT                                         0x0

#define HWIO_GSBI_QUP_MX_INPUT_COUNT_OFFSET                                                             (0x0200)
#define HWIO_GSBI_QUP_MX_INPUT_COUNT_RMSK                                                                 0xffff
#define HWIO_GSBI_QUP_MX_INPUT_COUNT_IN(GSBI_QUP_ADDR)\
   in_dword_masked((GSBI_QUP_ADDR)+HWIO_GSBI_QUP_MX_INPUT_COUNT_OFFSET, HWIO_GSBI_QUP_MX_INPUT_COUNT_RMSK)
#define HWIO_GSBI_QUP_MX_INPUT_COUNT_INM(GSBI_QUP_ADDR,m)\
   in_dword_masked((GSBI_QUP_ADDR)+HWIO_GSBI_QUP_MX_INPUT_COUNT_OFFSET, m)
#define HWIO_GSBI_QUP_MX_INPUT_COUNT_OUT(GSBI_QUP_ADDR,v)\
   out_dword((GSBI_QUP_ADDR)+HWIO_GSBI_QUP_MX_INPUT_COUNT_OFFSET, v)
#define HWIO_GSBI_QUP_MX_INPUT_COUNT_OUTM(GSBI_QUP_ADDR,m,v)\
   out_dword_masked_ns((GSBI_QUP_ADDR)+HWIO_GSBI_QUP_MX_INPUT_COUNT_OFFSET, m,v,HWIO_GSBI_QUP_MX_INPUT_COUNT_IN(GSBI_QUP_ADDR))
#define HWIO_GSBI_QUP_MX_INPUT_COUNT_MX_INPUT_COUNT_BMSK                                                  0xffff
#define HWIO_GSBI_QUP_MX_INPUT_COUNT_MX_INPUT_COUNT_SHFT                                                     0x0

#define HWIO_GSBI_QUP_MX_INPUT_CNT_CURRENT_OFFSET                                                       (0x0204)
#define HWIO_GSBI_QUP_MX_INPUT_CNT_CURRENT_RMSK                                                           0xffff
#define HWIO_GSBI_QUP_MX_INPUT_CNT_CURRENT_IN(GSBI_QUP_ADDR)\
   in_dword_masked((GSBI_QUP_ADDR)+HWIO_GSBI_QUP_MX_INPUT_CNT_CURRENT_OFFSET, HWIO_GSBI_QUP_MX_INPUT_CNT_CURRENT_RMSK)
#define HWIO_GSBI_QUP_MX_INPUT_CNT_CURRENT_INM(GSBI_QUP_ADDR,m)\
   in_dword_masked((GSBI_QUP_ADDR)+HWIO_GSBI_QUP_MX_INPUT_CNT_CURRENT_OFFSET, m)
#define HWIO_GSBI_QUP_MX_INPUT_CNT_CURRENT_MX_INPUT_CNT_CURRENT_BMSK                                      0xffff
#define HWIO_GSBI_QUP_MX_INPUT_CNT_CURRENT_MX_INPUT_CNT_CURRENT_SHFT                                         0x0

#define HWIO_GSBI_QUP_MX_READ_COUNT_OFFSET                                                              (0x0208)
#define HWIO_GSBI_QUP_MX_READ_COUNT_RMSK                                                                  0xffff
#define HWIO_GSBI_QUP_MX_READ_COUNT_IN(GSBI_QUP_ADDR)\
   in_dword_masked((GSBI_QUP_ADDR)+HWIO_GSBI_QUP_MX_READ_COUNT_OFFSET, HWIO_GSBI_QUP_MX_READ_COUNT_RMSK)
#define HWIO_GSBI_QUP_MX_READ_COUNT_INM(GSBI_QUP_ADDR,m)\
   in_dword_masked((GSBI_QUP_ADDR)+HWIO_GSBI_QUP_MX_READ_COUNT_OFFSET, m)
#define HWIO_GSBI_QUP_MX_READ_COUNT_OUT(GSBI_QUP_ADDR,v)\
   out_dword((GSBI_QUP_ADDR)+HWIO_GSBI_QUP_MX_READ_COUNT_OFFSET, v)
#define HWIO_GSBI_QUP_MX_READ_COUNT_OUTM(GSBI_QUP_ADDR,m,v)\
   out_dword_masked_ns((GSBI_QUP_ADDR)+HWIO_GSBI_QUP_MX_READ_COUNT_OFFSET, m,v,HWIO_GSBI_QUP_MX_READ_COUNT_IN(GSBI_QUP_ADDR))
#define HWIO_GSBI_QUP_MX_READ_COUNT_MX_READ_COUNT_BMSK                                                    0xffff
#define HWIO_GSBI_QUP_MX_READ_COUNT_MX_READ_COUNT_SHFT                                                       0x0

#define HWIO_GSBI_QUP_MX_READ_CNT_CURRENT_OFFSET                                                        (0x020C)
#define HWIO_GSBI_QUP_MX_READ_CNT_CURRENT_RMSK                                                            0xffff
#define HWIO_GSBI_QUP_MX_READ_CNT_CURRENT_IN(GSBI_QUP_ADDR)\
   in_dword_masked((GSBI_QUP_ADDR)+HWIO_GSBI_QUP_MX_READ_CNT_CURRENT_OFFSET, HWIO_GSBI_QUP_MX_READ_CNT_CURRENT_RMSK)
#define HWIO_GSBI_QUP_MX_READ_CNT_CURRENT_INM(GSBI_QUP_ADDR,m)\
   in_dword_masked((GSBI_QUP_ADDR)+HWIO_GSBI_QUP_MX_READ_CNT_CURRENT_OFFSET, m)
#define HWIO_GSBI_QUP_MX_READ_CNT_CURRENT_MX_READ_CNT_CURRENT_BMSK                                        0xffff
#define HWIO_GSBI_QUP_MX_READ_CNT_CURRENT_MX_READ_CNT_CURRENT_SHFT                                           0x0

#define HWIO_GSBI_QUP_INPUT_DEBUG_OFFSET                                                                (0x0210)
#define HWIO_GSBI_QUP_INPUT_DEBUG_RMSK                                                                0xffffffff
#define HWIO_GSBI_QUP_INPUT_DEBUG_IN(GSBI_QUP_ADDR)\
   in_dword_masked((GSBI_QUP_ADDR)+HWIO_GSBI_QUP_INPUT_DEBUG_OFFSET, HWIO_GSBI_QUP_INPUT_DEBUG_RMSK)
#define HWIO_GSBI_QUP_INPUT_DEBUG_INM(GSBI_QUP_ADDR,m)\
   in_dword_masked((GSBI_QUP_ADDR)+HWIO_GSBI_QUP_INPUT_DEBUG_OFFSET, m)
#define HWIO_GSBI_QUP_INPUT_DEBUG_INPUT_DEBUG_DATA_BMSK                                               0xffffffff
#define HWIO_GSBI_QUP_INPUT_DEBUG_INPUT_DEBUG_DATA_SHFT                                                      0x0

#define HWIO_GSBI_QUP_INPUT_FIFO_WORD_CNT_OFFSET                                                        (0x0214)
#define HWIO_GSBI_QUP_INPUT_FIFO_WORD_CNT_RMSK                                                             0x1ff
#define HWIO_GSBI_QUP_INPUT_FIFO_WORD_CNT_IN(GSBI_QUP_ADDR)\
   in_dword_masked((GSBI_QUP_ADDR)+HWIO_GSBI_QUP_INPUT_FIFO_WORD_CNT_OFFSET, HWIO_GSBI_QUP_INPUT_FIFO_WORD_CNT_RMSK)
#define HWIO_GSBI_QUP_INPUT_FIFO_WORD_CNT_INM(GSBI_QUP_ADDR,m)\
   in_dword_masked((GSBI_QUP_ADDR)+HWIO_GSBI_QUP_INPUT_FIFO_WORD_CNT_OFFSET, m)
#define HWIO_GSBI_QUP_INPUT_FIFO_WORD_CNT_INPUT_FIFO_WORD_CNT_BMSK                                         0x1ff
#define HWIO_GSBI_QUP_INPUT_FIFO_WORD_CNT_INPUT_FIFO_WORD_CNT_SHFT                                           0x0

#define HWIO_GSBI_QUP_INPUT_FIFO0_OFFSET                                                                (0x0218)
#define HWIO_GSBI_QUP_INPUT_FIFO0_RMSK                                                                0xffffffff
#define HWIO_GSBI_QUP_INPUT_FIFO0_IN(GSBI_QUP_ADDR)\
   in_dword_masked((GSBI_QUP_ADDR)+HWIO_GSBI_QUP_INPUT_FIFO0_OFFSET, HWIO_GSBI_QUP_INPUT_FIFO0_RMSK)
#define HWIO_GSBI_QUP_INPUT_FIFO0_INM(GSBI_QUP_ADDR,m)\
   in_dword_masked((GSBI_QUP_ADDR)+HWIO_GSBI_QUP_INPUT_FIFO0_OFFSET, m)
#define HWIO_GSBI_QUP_INPUT_FIFO0_INPUT_BMSK                                                          0xffffffff
#define HWIO_GSBI_QUP_INPUT_FIFO0_INPUT_SHFT                                                                 0x0

#define HWIO_GSBI_SPI_CONFIG_OFFSET                                                                     (0x0300)
#define HWIO_GSBI_SPI_CONFIG_RMSK                                                                          0x720
#define HWIO_GSBI_SPI_CONFIG_IN(GSBI_QUP_ADDR)\
   in_dword_masked((GSBI_QUP_ADDR)+HWIO_GSBI_SPI_CONFIG_OFFSET, HWIO_GSBI_SPI_CONFIG_RMSK)
#define HWIO_GSBI_SPI_CONFIG_INM(GSBI_QUP_ADDR,m)\
   in_dword_masked((GSBI_QUP_ADDR)+HWIO_GSBI_SPI_CONFIG_OFFSET, m)
#define HWIO_GSBI_SPI_CONFIG_OUT(GSBI_QUP_ADDR,v)\
   out_dword((GSBI_QUP_ADDR)+HWIO_GSBI_SPI_CONFIG_OFFSET, v)
#define HWIO_GSBI_SPI_CONFIG_OUTM(GSBI_QUP_ADDR,m,v)\
   out_dword_masked_ns((GSBI_QUP_ADDR)+HWIO_GSBI_SPI_CONFIG_OFFSET, m,v,HWIO_GSBI_SPI_CONFIG_IN(GSBI_QUP_ADDR))
#define HWIO_GSBI_SPI_CONFIG_HS_MODE_BMSK                                                                  0x400
#define HWIO_GSBI_SPI_CONFIG_HS_MODE_SHFT                                                                    0xa
#define HWIO_GSBI_SPI_CONFIG_INPUT_FIRST_BMSK                                                              0x200
#define HWIO_GSBI_SPI_CONFIG_INPUT_FIRST_SHFT                                                                0x9
#define HWIO_GSBI_SPI_CONFIG_LOOP_BACK_BMSK                                                                0x100
#define HWIO_GSBI_SPI_CONFIG_LOOP_BACK_SHFT                                                                  0x8
#define HWIO_GSBI_SPI_CONFIG_SLAVE_OPERATION_BMSK                                                           0x20
#define HWIO_GSBI_SPI_CONFIG_SLAVE_OPERATION_SHFT                                                            0x5

#define HWIO_GSBI_SPI_IO_CONTROL_OFFSET                                                                 (0x0304)
#define HWIO_GSBI_SPI_IO_CONTROL_RMSK                                                                      0xfff
#define HWIO_GSBI_SPI_IO_CONTROL_IN(GSBI_QUP_ADDR)\
   in_dword_masked((GSBI_QUP_ADDR)+HWIO_GSBI_SPI_IO_CONTROL_OFFSET, HWIO_GSBI_SPI_IO_CONTROL_RMSK)
#define HWIO_GSBI_SPI_IO_CONTROL_INM(GSBI_QUP_ADDR,m)\
   in_dword_masked((GSBI_QUP_ADDR)+HWIO_GSBI_SPI_IO_CONTROL_OFFSET, m)
#define HWIO_GSBI_SPI_IO_CONTROL_OUT(GSBI_QUP_ADDR,v)\
   out_dword((GSBI_QUP_ADDR)+HWIO_GSBI_SPI_IO_CONTROL_OFFSET, v)
#define HWIO_GSBI_SPI_IO_CONTROL_OUTM(GSBI_QUP_ADDR,m,v)\
   out_dword_masked_ns((GSBI_QUP_ADDR)+HWIO_GSBI_SPI_IO_CONTROL_OFFSET, m,v,HWIO_GSBI_SPI_IO_CONTROL_IN(GSBI_QUP_ADDR))
#define HWIO_GSBI_SPI_IO_CONTROL_FORCE_CS_BMSK                                                             0x800
#define HWIO_GSBI_SPI_IO_CONTROL_FORCE_CS_SHFT                                                               0xb
#define HWIO_GSBI_SPI_IO_CONTROL_CLK_IDLE_HIGH_BMSK                                                        0x400
#define HWIO_GSBI_SPI_IO_CONTROL_CLK_IDLE_HIGH_SHFT                                                          0xa
#define HWIO_GSBI_SPI_IO_CONTROL_CLK_ALWAYS_ON_BMSK                                                        0x200
#define HWIO_GSBI_SPI_IO_CONTROL_CLK_ALWAYS_ON_SHFT                                                          0x9
#define HWIO_GSBI_SPI_IO_CONTROL_MX_CS_MODE_BMSK                                                           0x100
#define HWIO_GSBI_SPI_IO_CONTROL_MX_CS_MODE_SHFT                                                             0x8
#define HWIO_GSBI_SPI_IO_CONTROL_CS_N_POLARITY_BMSK                                                         0xf0
#define HWIO_GSBI_SPI_IO_CONTROL_CS_N_POLARITY_SHFT                                                          0x4
#define HWIO_GSBI_SPI_IO_CONTROL_CS_SELECT_BMSK                                                              0xc
#define HWIO_GSBI_SPI_IO_CONTROL_CS_SELECT_SHFT                                                              0x2
#define HWIO_GSBI_SPI_IO_CONTROL_TRISTATE_CS_BMSK                                                            0x2
#define HWIO_GSBI_SPI_IO_CONTROL_TRISTATE_CS_SHFT                                                            0x1
#define HWIO_GSBI_SPI_IO_CONTROL_NO_TRI_STATE_BMSK                                                           0x1
#define HWIO_GSBI_SPI_IO_CONTROL_NO_TRI_STATE_SHFT                                                           0x0

#define HWIO_GSBI_SPI_ERROR_FLAGS_OFFSET                                                                (0x0308)
#define HWIO_GSBI_SPI_ERROR_FLAGS_RMSK                                                                       0x3
#define HWIO_GSBI_SPI_ERROR_FLAGS_IN(GSBI_QUP_ADDR)\
   in_dword_masked((GSBI_QUP_ADDR)+HWIO_GSBI_SPI_ERROR_FLAGS_OFFSET, HWIO_GSBI_SPI_ERROR_FLAGS_RMSK)
#define HWIO_GSBI_SPI_ERROR_FLAGS_INM(GSBI_QUP_ADDR,m)\
   in_dword_masked((GSBI_QUP_ADDR)+HWIO_GSBI_SPI_ERROR_FLAGS_OFFSET, m)
#define HWIO_GSBI_SPI_ERROR_FLAGS_OUT(GSBI_QUP_ADDR,v)\
   out_dword((GSBI_QUP_ADDR)+HWIO_GSBI_SPI_ERROR_FLAGS_OFFSET, v)
#define HWIO_GSBI_SPI_ERROR_FLAGS_OUTM(GSBI_QUP_ADDR,m,v)\
   out_dword_masked_ns((GSBI_QUP_ADDR)+HWIO_GSBI_SPI_ERROR_FLAGS_OFFSET, m,v,HWIO_GSBI_SPI_ERROR_FLAGS_IN(GSBI_QUP_ADDR))
#define HWIO_GSBI_SPI_ERROR_FLAGS_SPI_SLV_CLK_OVER_RUN_ERR_BMSK                                              0x2
#define HWIO_GSBI_SPI_ERROR_FLAGS_SPI_SLV_CLK_OVER_RUN_ERR_SHFT                                              0x1
#define HWIO_GSBI_SPI_ERROR_FLAGS_SPI_SLV_CLK_UNDER_RUN_ERR_BMSK                                             0x1
#define HWIO_GSBI_SPI_ERROR_FLAGS_SPI_SLV_CLK_UNDER_RUN_ERR_SHFT                                             0x0

#define HWIO_GSBI_SPI_ERROR_FLAGS_EN_OFFSET                                                             (0x030C)
#define HWIO_GSBI_SPI_ERROR_FLAGS_EN_RMSK                                                                    0x3
#define HWIO_GSBI_SPI_ERROR_FLAGS_EN_IN(GSBI_QUP_ADDR)\
   in_dword_masked((GSBI_QUP_ADDR)+HWIO_GSBI_SPI_ERROR_FLAGS_EN_OFFSET, HWIO_GSBI_SPI_ERROR_FLAGS_EN_RMSK)
#define HWIO_GSBI_SPI_ERROR_FLAGS_EN_INM(GSBI_QUP_ADDR,m)\
   in_dword_masked((GSBI_QUP_ADDR)+HWIO_GSBI_SPI_ERROR_FLAGS_EN_OFFSET, m)
#define HWIO_GSBI_SPI_ERROR_FLAGS_EN_OUT(GSBI_QUP_ADDR,v)\
   out_dword((GSBI_QUP_ADDR)+HWIO_GSBI_SPI_ERROR_FLAGS_EN_OFFSET, v)
#define HWIO_GSBI_SPI_ERROR_FLAGS_EN_OUTM(GSBI_QUP_ADDR,m,v)\
   out_dword_masked_ns((GSBI_QUP_ADDR)+HWIO_GSBI_SPI_ERROR_FLAGS_EN_OFFSET, m,v,HWIO_GSBI_SPI_ERROR_FLAGS_EN_IN(GSBI_QUP_ADDR))
#define HWIO_GSBI_SPI_ERROR_FLAGS_EN_SPI_SLV_CLK_OVER_RUN_ERR_EN_BMSK                                        0x2
#define HWIO_GSBI_SPI_ERROR_FLAGS_EN_SPI_SLV_CLK_OVER_RUN_ERR_EN_SHFT                                        0x1
#define HWIO_GSBI_SPI_ERROR_FLAGS_EN_SPI_SLV_CLK_UNDER_RUN_ERR_EN_BMSK                                       0x1
#define HWIO_GSBI_SPI_ERROR_FLAGS_EN_SPI_SLV_CLK_UNDER_RUN_ERR_EN_SHFT                                       0x0

#define HWIO_GSBI_SPI_DEASSERT_WAIT_OFFSET                                                              (0x0310)
#define HWIO_GSBI_SPI_DEASSERT_WAIT_RMSK                                                                    0x3f
#define HWIO_GSBI_SPI_DEASSERT_WAIT_IN(GSBI_QUP_ADDR)\
   in_dword_masked((GSBI_QUP_ADDR)+HWIO_GSBI_SPI_DEASSERT_WAIT_OFFSET, HWIO_GSBI_SPI_DEASSERT_WAIT_RMSK)
#define HWIO_GSBI_SPI_DEASSERT_WAIT_INM(GSBI_QUP_ADDR,m)\
   in_dword_masked((GSBI_QUP_ADDR)+HWIO_GSBI_SPI_DEASSERT_WAIT_OFFSET, m)
#define HWIO_GSBI_SPI_DEASSERT_WAIT_OUT(GSBI_QUP_ADDR,v)\
   out_dword((GSBI_QUP_ADDR)+HWIO_GSBI_SPI_DEASSERT_WAIT_OFFSET, v)
#define HWIO_GSBI_SPI_DEASSERT_WAIT_OUTM(GSBI_QUP_ADDR,m,v)\
   out_dword_masked_ns((GSBI_QUP_ADDR)+HWIO_GSBI_SPI_DEASSERT_WAIT_OFFSET, m,v,HWIO_GSBI_SPI_DEASSERT_WAIT_IN(GSBI_QUP_ADDR))
#define HWIO_GSBI_SPI_DEASSERT_WAIT_DEASSERT_WAIT_BMSK                                                      0x3f
#define HWIO_GSBI_SPI_DEASSERT_WAIT_DEASSERT_WAIT_SHFT                                                       0x0

#define HWIO_GSBI_I2C_MASTER_CLK_CTL_OFFSET                                                             (0x0400)
#define HWIO_GSBI_I2C_MASTER_CLK_CTL_RMSK                                                              0xfff07ff
#define HWIO_GSBI_I2C_MASTER_CLK_CTL_IN(GSBI_QUP_ADDR)\
   in_dword_masked((GSBI_QUP_ADDR)+HWIO_GSBI_I2C_MASTER_CLK_CTL_OFFSET, HWIO_GSBI_I2C_MASTER_CLK_CTL_RMSK)
#define HWIO_GSBI_I2C_MASTER_CLK_CTL_INM(GSBI_QUP_ADDR,m)\
   in_dword_masked((GSBI_QUP_ADDR)+HWIO_GSBI_I2C_MASTER_CLK_CTL_OFFSET, m)
#define HWIO_GSBI_I2C_MASTER_CLK_CTL_OUT(GSBI_QUP_ADDR,v)\
   out_dword((GSBI_QUP_ADDR)+HWIO_GSBI_I2C_MASTER_CLK_CTL_OFFSET, v)
#define HWIO_GSBI_I2C_MASTER_CLK_CTL_OUTM(GSBI_QUP_ADDR,m,v)\
   out_dword_masked_ns((GSBI_QUP_ADDR)+HWIO_GSBI_I2C_MASTER_CLK_CTL_OFFSET, m,v,HWIO_GSBI_I2C_MASTER_CLK_CTL_IN(GSBI_QUP_ADDR))
#define HWIO_GSBI_I2C_MASTER_CLK_CTL_SDA_NOISE_REJECTION_BMSK                                          0xc000000
#define HWIO_GSBI_I2C_MASTER_CLK_CTL_SDA_NOISE_REJECTION_SHFT                                               0x1a
#define HWIO_GSBI_I2C_MASTER_CLK_CTL_SCL_NOISE_REJECTION_BMSK                                          0x3000000
#define HWIO_GSBI_I2C_MASTER_CLK_CTL_SCL_NOISE_REJECTION_SHFT                                               0x18
#define HWIO_GSBI_I2C_MASTER_CLK_CTL_HIGH_TIME_DIVIDER_VALUE_BMSK                                       0xff0000
#define HWIO_GSBI_I2C_MASTER_CLK_CTL_HIGH_TIME_DIVIDER_VALUE_SHFT                                           0x10
#define HWIO_GSBI_I2C_MASTER_CLK_CTL_HS_DIVIDER_VALUE_BMSK                                                 0x700
#define HWIO_GSBI_I2C_MASTER_CLK_CTL_HS_DIVIDER_VALUE_SHFT                                                   0x8
#define HWIO_GSBI_I2C_MASTER_CLK_CTL_FS_DIVIDER_VALUE_BMSK                                                  0xff
#define HWIO_GSBI_I2C_MASTER_CLK_CTL_FS_DIVIDER_VALUE_SHFT                                                   0x0

#define HWIO_GSBI_I2C_MASTER_STATUS_OFFSET                                                              (0x0404)
#define HWIO_GSBI_I2C_MASTER_STATUS_RMSK                                                               0xfbffffc
#define HWIO_GSBI_I2C_MASTER_STATUS_IN(GSBI_QUP_ADDR)\
   in_dword_masked((GSBI_QUP_ADDR)+HWIO_GSBI_I2C_MASTER_STATUS_OFFSET, HWIO_GSBI_I2C_MASTER_STATUS_RMSK)
#define HWIO_GSBI_I2C_MASTER_STATUS_INM(GSBI_QUP_ADDR,m)\
   in_dword_masked((GSBI_QUP_ADDR)+HWIO_GSBI_I2C_MASTER_STATUS_OFFSET, m)
#define HWIO_GSBI_I2C_MASTER_STATUS_OUT(GSBI_QUP_ADDR,v)\
   out_dword((GSBI_QUP_ADDR)+HWIO_GSBI_I2C_MASTER_STATUS_OFFSET, v)
#define HWIO_GSBI_I2C_MASTER_STATUS_OUTM(GSBI_QUP_ADDR,m,v)\
   out_dword_masked_ns((GSBI_QUP_ADDR)+HWIO_GSBI_I2C_MASTER_STATUS_OFFSET, m,v,HWIO_GSBI_I2C_MASTER_STATUS_IN(GSBI_QUP_ADDR))
#define HWIO_GSBI_I2C_MASTER_STATUS_I2C_SCL_BMSK                                                       0x8000000
#define HWIO_GSBI_I2C_MASTER_STATUS_I2C_SCL_SHFT                                                            0x1b
#define HWIO_GSBI_I2C_MASTER_STATUS_I2C_SDA_BMSK                                                       0x4000000
#define HWIO_GSBI_I2C_MASTER_STATUS_I2C_SDA_SHFT                                                            0x1a
#define HWIO_GSBI_I2C_MASTER_STATUS_INVALID_READ_SEQ_BMSK                                              0x2000000
#define HWIO_GSBI_I2C_MASTER_STATUS_INVALID_READ_SEQ_SHFT                                                   0x19
#define HWIO_GSBI_I2C_MASTER_STATUS_INVALID_READ_ADDR_BMSK                                             0x1000000
#define HWIO_GSBI_I2C_MASTER_STATUS_INVALID_READ_ADDR_SHFT                                                  0x18
#define HWIO_GSBI_I2C_MASTER_STATUS_INVALID_TAG_BMSK                                                    0x800000
#define HWIO_GSBI_I2C_MASTER_STATUS_INVALID_TAG_SHFT                                                        0x17
#define HWIO_GSBI_I2C_MASTER_STATUS_INPUT_FSM_STATE_BMSK                                                0x380000
#define HWIO_GSBI_I2C_MASTER_STATUS_INPUT_FSM_STATE_SHFT                                                    0x13
#define HWIO_GSBI_I2C_MASTER_STATUS_OUTPUT_FSM_STATE_BMSK                                                0x70000
#define HWIO_GSBI_I2C_MASTER_STATUS_OUTPUT_FSM_STATE_SHFT                                                   0x10
#define HWIO_GSBI_I2C_MASTER_STATUS_CLK_STATE_BMSK                                                        0xe000
#define HWIO_GSBI_I2C_MASTER_STATUS_CLK_STATE_SHFT                                                           0xd
#define HWIO_GSBI_I2C_MASTER_STATUS_DATA_STATE_BMSK                                                       0x1c00
#define HWIO_GSBI_I2C_MASTER_STATUS_DATA_STATE_SHFT                                                          0xa
#define HWIO_GSBI_I2C_MASTER_STATUS_BUS_MASTER_BMSK                                                        0x200
#define HWIO_GSBI_I2C_MASTER_STATUS_BUS_MASTER_SHFT                                                          0x9
#define HWIO_GSBI_I2C_MASTER_STATUS_BUS_ACTIVE_BMSK                                                        0x100
#define HWIO_GSBI_I2C_MASTER_STATUS_BUS_ACTIVE_SHFT                                                          0x8
#define HWIO_GSBI_I2C_MASTER_STATUS_FAILED_BMSK                                                             0xc0
#define HWIO_GSBI_I2C_MASTER_STATUS_FAILED_SHFT                                                              0x6
#define HWIO_GSBI_I2C_MASTER_STATUS_INVALID_WRITE_BMSK                                                      0x20
#define HWIO_GSBI_I2C_MASTER_STATUS_INVALID_WRITE_SHFT                                                       0x5
#define HWIO_GSBI_I2C_MASTER_STATUS_ARB_LOST_BMSK                                                           0x10
#define HWIO_GSBI_I2C_MASTER_STATUS_ARB_LOST_SHFT                                                            0x4
#define HWIO_GSBI_I2C_MASTER_STATUS_PACKET_NACKED_BMSK                                                       0x8
#define HWIO_GSBI_I2C_MASTER_STATUS_PACKET_NACKED_SHFT                                                       0x3
#define HWIO_GSBI_I2C_MASTER_STATUS_BUS_ERROR_BMSK                                                           0x4
#define HWIO_GSBI_I2C_MASTER_STATUS_BUS_ERROR_SHFT                                                           0x2

#define HWIO_GSBI_I2C_MASTER_CONFIG_OFFSET                                                              (0x0408)
#define HWIO_GSBI_I2C_MASTER_CONFIG_RMSK                                                                     0x3
#define HWIO_GSBI_I2C_MASTER_CONFIG_IN(GSBI_QUP_ADDR)\
   in_dword_masked((GSBI_QUP_ADDR)+HWIO_GSBI_I2C_MASTER_CONFIG_OFFSET, HWIO_GSBI_I2C_MASTER_CONFIG_RMSK)
#define HWIO_GSBI_I2C_MASTER_CONFIG_INM(GSBI_QUP_ADDR,m)\
   in_dword_masked((GSBI_QUP_ADDR)+HWIO_GSBI_I2C_MASTER_CONFIG_OFFSET, m)
#define HWIO_GSBI_I2C_MASTER_CONFIG_OUT(GSBI_QUP_ADDR,v)\
   out_dword((GSBI_QUP_ADDR)+HWIO_GSBI_I2C_MASTER_CONFIG_OFFSET, v)
#define HWIO_GSBI_I2C_MASTER_CONFIG_OUTM(GSBI_QUP_ADDR,m,v)\
   out_dword_masked_ns((GSBI_QUP_ADDR)+HWIO_GSBI_I2C_MASTER_CONFIG_OFFSET, m,v,HWIO_GSBI_I2C_MASTER_CONFIG_IN(GSBI_QUP_ADDR))
#define HWIO_GSBI_I2C_MASTER_CONFIG_EN_DIRECT_ACCESS_BMSK                                                    0x2
#define HWIO_GSBI_I2C_MASTER_CONFIG_EN_DIRECT_ACCESS_SHFT                                                    0x1
#define HWIO_GSBI_I2C_MASTER_CONFIG_EN_VERSION_TWO_TAG_BMSK                                                  0x1
#define HWIO_GSBI_I2C_MASTER_CONFIG_EN_VERSION_TWO_TAG_SHFT                                                  0x0

#define HWIO_GSBI_I2C_MASTER_BUS_CLEAR_OFFSET                                                           (0x040C)
#define HWIO_GSBI_I2C_MASTER_BUS_CLEAR_RMSK                                                                  0x1
#define HWIO_GSBI_I2C_MASTER_BUS_CLEAR_IN(GSBI_QUP_ADDR)\
   in_dword_masked((GSBI_QUP_ADDR)+HWIO_GSBI_I2C_MASTER_BUS_CLEAR_OFFSET, HWIO_GSBI_I2C_MASTER_BUS_CLEAR_RMSK)
#define HWIO_GSBI_I2C_MASTER_BUS_CLEAR_INM(GSBI_QUP_ADDR,m)\
   in_dword_masked((GSBI_QUP_ADDR)+HWIO_GSBI_I2C_MASTER_BUS_CLEAR_OFFSET, m)
#define HWIO_GSBI_I2C_MASTER_BUS_CLEAR_OUT(GSBI_QUP_ADDR,v)\
   out_dword((GSBI_QUP_ADDR)+HWIO_GSBI_I2C_MASTER_BUS_CLEAR_OFFSET, v)
#define HWIO_GSBI_I2C_MASTER_BUS_CLEAR_OUTM(GSBI_QUP_ADDR,m,v)\
   out_dword_masked_ns((GSBI_QUP_ADDR)+HWIO_GSBI_I2C_MASTER_BUS_CLEAR_OFFSET, m,v,HWIO_GSBI_I2C_MASTER_BUS_CLEAR_IN(GSBI_QUP_ADDR))
#define HWIO_GSBI_I2C_MASTER_BUS_CLEAR_CLEAR_BMSK                                                            0x1
#define HWIO_GSBI_I2C_MASTER_BUS_CLEAR_CLEAR_SHFT                                                            0x0

#define HWIO_GSBI_I2C_MASTER_LINE_STATE_OFFSET                                                          (0x0410)
#define HWIO_GSBI_I2C_MASTER_LINE_STATE_RMSK                                                                 0x3
#define HWIO_GSBI_I2C_MASTER_LINE_STATE_IN(GSBI_QUP_ADDR)\
   in_dword_masked((GSBI_QUP_ADDR)+HWIO_GSBI_I2C_MASTER_LINE_STATE_OFFSET, HWIO_GSBI_I2C_MASTER_LINE_STATE_RMSK)
#define HWIO_GSBI_I2C_MASTER_LINE_STATE_INM(GSBI_QUP_ADDR,m)\
   in_dword_masked((GSBI_QUP_ADDR)+HWIO_GSBI_I2C_MASTER_LINE_STATE_OFFSET, m)
#define HWIO_GSBI_I2C_MASTER_LINE_STATE_SCL_BMSK                                                             0x2
#define HWIO_GSBI_I2C_MASTER_LINE_STATE_SCL_SHFT                                                             0x1
#define HWIO_GSBI_I2C_MASTER_LINE_STATE_SDA_BMSK                                                             0x1
#define HWIO_GSBI_I2C_MASTER_LINE_STATE_SDA_SHFT                                                             0x0

#define HWIO_GSBI_I2C_WRITE_DATA_OFFSET                                                                 (0x0414)
#define HWIO_GSBI_I2C_WRITE_DATA_RMSK                                                                      0x3ff
#define HWIO_GSBI_I2C_WRITE_DATA_IN(GSBI_QUP_ADDR)\
   in_dword_masked((GSBI_QUP_ADDR)+HWIO_GSBI_I2C_WRITE_DATA_OFFSET, HWIO_GSBI_I2C_WRITE_DATA_RMSK)
#define HWIO_GSBI_I2C_WRITE_DATA_INM(GSBI_QUP_ADDR,m)\
   in_dword_masked((GSBI_QUP_ADDR)+HWIO_GSBI_I2C_WRITE_DATA_OFFSET, m)
#define HWIO_GSBI_I2C_WRITE_DATA_OUT(GSBI_QUP_ADDR,v)\
   out_dword((GSBI_QUP_ADDR)+HWIO_GSBI_I2C_WRITE_DATA_OFFSET, v)
#define HWIO_GSBI_I2C_WRITE_DATA_OUTM(GSBI_QUP_ADDR,m,v)\
   out_dword_masked_ns((GSBI_QUP_ADDR)+HWIO_GSBI_I2C_WRITE_DATA_OFFSET, m,v,HWIO_GSBI_I2C_WRITE_DATA_IN(GSBI_QUP_ADDR))
#define HWIO_GSBI_I2C_WRITE_DATA_LAST_BYTE_BMSK                                                            0x200
#define HWIO_GSBI_I2C_WRITE_DATA_LAST_BYTE_SHFT                                                              0x9
#define HWIO_GSBI_I2C_WRITE_DATA_ADDR_BYTE_BMSK                                                            0x100
#define HWIO_GSBI_I2C_WRITE_DATA_ADDR_BYTE_SHFT                                                              0x8
#define HWIO_GSBI_I2C_WRITE_DATA_DATA_BYTE_BMSK                                                             0xff
#define HWIO_GSBI_I2C_WRITE_DATA_DATA_BYTE_SHFT                                                              0x0

#define HWIO_GSBI_I2C_READ_DATA_OFFSET                                                                  (0x0418)
#define HWIO_GSBI_I2C_READ_DATA_RMSK                                                                        0xff
#define HWIO_GSBI_I2C_READ_DATA_IN(GSBI_QUP_ADDR)\
   in_dword_masked((GSBI_QUP_ADDR)+HWIO_GSBI_I2C_READ_DATA_OFFSET, HWIO_GSBI_I2C_READ_DATA_RMSK)
#define HWIO_GSBI_I2C_READ_DATA_INM(GSBI_QUP_ADDR,m)\
   in_dword_masked((GSBI_QUP_ADDR)+HWIO_GSBI_I2C_READ_DATA_OFFSET, m)
#define HWIO_GSBI_I2C_READ_DATA_I2C_READ_DATA_BMSK                                                          0xff
#define HWIO_GSBI_I2C_READ_DATA_I2C_READ_DATA_SHFT                                                           0x0

/* TCSR BITS, for QUP INTERRUPTs in MPSS*/
#define HWIO_PHSS_QUP_MSS_INT_SEL_n_ADDR(base,n)                                                                          ((base) + 0x0000b140 + 0x10 * (n))
#define HWIO_PHSS_QUP_MSS_INT_SEL_n_OFFS(base,n)                                                                          (0x0000b140 + 0x10 * (n))
#define HWIO_PHSS_QUP_MSS_INT_SEL_n_RMSK                                                                                       0xfff
#define HWIO_PHSS_QUP_MSS_INT_SEL_n_MAXn                                                                                           1
#define HWIO_PHSS_QUP_MSS_INT_SEL_n_INI(base,n)        \
        in_dword_masked(HWIO_PHSS_QUP_MSS_INT_SEL_n_ADDR(base,n), HWIO_PHSS_QUP_MSS_INT_SEL_n_RMSK)
#define HWIO_PHSS_QUP_MSS_INT_SEL_n_INMI(base,n,mask)    \
        in_dword_masked(HWIO_PHSS_QUP_MSS_INT_SEL_n_ADDR(base,n), mask)
#define HWIO_PHSS_QUP_MSS_INT_SEL_n_OUTI(base,n,val)    \
        out_dword(HWIO_PHSS_QUP_MSS_INT_SEL_n_ADDR(base,n),val)
#define HWIO_PHSS_QUP_MSS_INT_SEL_n_OUTMI(base,n,mask,val) \
        out_dword_masked_ns(HWIO_PHSS_QUP_MSS_INT_SEL_n_ADDR(base,n),mask,val,HWIO_PHSS_QUP_MSS_INT_SEL_n_INI(base,n))
#define HWIO_PHSS_QUP_MSS_INT_SEL_n_MSS_BLSP2_QUP_6_IRQ_ENABLE_BMSK                                                            0x800
#define HWIO_PHSS_QUP_MSS_INT_SEL_n_MSS_BLSP2_QUP_6_IRQ_ENABLE_SHFT                                                              0xb
#define HWIO_PHSS_QUP_MSS_INT_SEL_n_MSS_BLSP2_QUP_5_IRQ_ENABLE_BMSK                                                            0x400
#define HWIO_PHSS_QUP_MSS_INT_SEL_n_MSS_BLSP2_QUP_5_IRQ_ENABLE_SHFT                                                              0xa
#define HWIO_PHSS_QUP_MSS_INT_SEL_n_MSS_BLSP2_QUP_4_IRQ_ENABLE_BMSK                                                            0x200
#define HWIO_PHSS_QUP_MSS_INT_SEL_n_MSS_BLSP2_QUP_4_IRQ_ENABLE_SHFT                                                              0x9
#define HWIO_PHSS_QUP_MSS_INT_SEL_n_MSS_BLSP2_QUP_3_IRQ_ENABLE_BMSK                                                            0x100
#define HWIO_PHSS_QUP_MSS_INT_SEL_n_MSS_BLSP2_QUP_3_IRQ_ENABLE_SHFT                                                              0x8
#define HWIO_PHSS_QUP_MSS_INT_SEL_n_MSS_BLSP2_QUP_2_IRQ_ENABLE_BMSK                                                             0x80
#define HWIO_PHSS_QUP_MSS_INT_SEL_n_MSS_BLSP2_QUP_2_IRQ_ENABLE_SHFT                                                              0x7
#define HWIO_PHSS_QUP_MSS_INT_SEL_n_MSS_BLSP2_QUP_1_IRQ_ENABLE_BMSK                                                             0x40
#define HWIO_PHSS_QUP_MSS_INT_SEL_n_MSS_BLSP2_QUP_1_IRQ_ENABLE_SHFT                                                              0x6
#define HWIO_PHSS_QUP_MSS_INT_SEL_n_MSS_BLSP1_QUP_6_IRQ_ENABLE_BMSK                                                             0x20
#define HWIO_PHSS_QUP_MSS_INT_SEL_n_MSS_BLSP1_QUP_6_IRQ_ENABLE_SHFT                                                              0x5
#define HWIO_PHSS_QUP_MSS_INT_SEL_n_MSS_BLSP1_QUP_5_IRQ_ENABLE_BMSK                                                             0x10
#define HWIO_PHSS_QUP_MSS_INT_SEL_n_MSS_BLSP1_QUP_5_IRQ_ENABLE_SHFT                                                              0x4
#define HWIO_PHSS_QUP_MSS_INT_SEL_n_MSS_BLSP1_QUP_4_IRQ_ENABLE_BMSK                                                              0x8
#define HWIO_PHSS_QUP_MSS_INT_SEL_n_MSS_BLSP1_QUP_4_IRQ_ENABLE_SHFT                                                              0x3
#define HWIO_PHSS_QUP_MSS_INT_SEL_n_MSS_BLSP1_QUP_3_IRQ_ENABLE_BMSK                                                              0x4
#define HWIO_PHSS_QUP_MSS_INT_SEL_n_MSS_BLSP1_QUP_3_IRQ_ENABLE_SHFT                                                              0x2
#define HWIO_PHSS_QUP_MSS_INT_SEL_n_MSS_BLSP1_QUP_2_IRQ_ENABLE_BMSK                                                              0x2
#define HWIO_PHSS_QUP_MSS_INT_SEL_n_MSS_BLSP1_QUP_2_IRQ_ENABLE_SHFT                                                              0x1
#define HWIO_PHSS_QUP_MSS_INT_SEL_n_MSS_BLSP1_QUP_1_IRQ_ENABLE_BMSK                                                              0x1
#define HWIO_PHSS_QUP_MSS_INT_SEL_n_MSS_BLSP1_QUP_1_IRQ_ENABLE_SHFT                                                              0x0
#endif /* __HAL_HALHWIOQUP_H__ */
