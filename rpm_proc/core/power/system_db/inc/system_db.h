/*============================================================================
  FILE:         system_db.h
 
  OVERVIEW:     Define structures and constants used by all implementations of
                systemdb.
 
  DEPENDENCIES: None
 
                Copyright (c) 2014 Qualcomm Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary

$Header: //components/rel/rpm.bf/2.1.1/core/power/system_db/inc/system_db.h#1 $
$Date: 2015/02/25 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/04/14   dy      Add Downsampling Feature
09/28/14   dy      Add GPIO, RPM DUMP, ERR FATAL commands
                   Include SystemDB Pro in internal builds by default

============================================================================*/
#ifndef SYSTEMDB_H
#define SYSTEMDB_H
#include "DALStdDef.h"

#define SYSTEMDB_READ_SIZE 3
#define SYSTEMDB_WRITE_SIZE 5
#define SYSTEMDB_CLEARALL_SIZE 1
#define SYSTEMDB_ENDOFCMDS_SIZE 1

#define CLEAR_ALL   0xFFFFFFFF
#define END_OF_CMD	0xF0F0F0F0

typedef enum
{ //bitmask -- for event types.  
  SYSTEMDB_UNUSED = 0,  //internal use only
  SYSTEMDB_IMMEDIATE = 0x1, //use immediately.  Will not be put in op queue
  SYSTEMDB_ENTER_XO = 0x2,
  SYSTEMDB_EXIT_XO = 0x4,
  SYSTEMDB_ALL_XO = 0x6,
  SYSTEMDB_ENTER_HALT = 0x8, 
  SYSTEMDB_EXIT_HALT = 0x10,
  SYSTEMDB_ALL_HALT = 0x18,
  SYSTEMDB_ENTER_VDDMIN = 0x20,
  SYSTEMDB_EXIT_VDDMIN = 0x40,
  SYSTEMDB_ALL_VDDMIN = 0x60,
} SYSTEMDB_EVENT_TYPE;

/* Type of address being read PMIC, non_PMIC. Used to determine if this is an 
   SPMI operation or direct access */
typedef enum
{
  SYSTEMDB_MEMORY = 1,       
  SYSTEMDB_PMIC,
  SYSTEMDB_CLK,
  SYSTEMDB_GPIO,
  SYSTEMDB_RPMDUMP,
  SYSTEMDB_ERRFATAL_CMD, // Repurposing to SystemDB Meta Commands
  SYSTEMDB_ENABLE_PRO,
  SYSTEMDB_DOWNSAMPLE,
} SYSTEMDB_ADDR_TYPE;

/* Read or write bit.  This will be masked into the type field in the 
 * operation element structure */
typedef enum
{
  SYSTEMDB_OP_READ = 0,
  SYSTEMDB_OP_WRITE,
} SYSTEMDB_OP_TYPE;

typedef struct SYSTEMDB_OP_QUEUE_ELEM
{
  struct SYSTEMDB_OP_QUEUE_ELEM *next;
  uint32 type;        /* 16 MSB = command_id
                       * next 1b = read/write
                       * next 4b = address type
                       * last 11b = event type
                       */
  /* Address to be written */
  uint32 addr;
  /* Number of register reads/writes to be performed, sequentially starting at addr */
  uint32 num; 
  /* If op type is write, data to be written to the specified address, -
     otherwise, not used */
  uint32 data;
  /* If op type is write, mask to apply to data for writing, otherwise unused
     i.e. allows to differentiate what bits of the data should be applied and 
     which are don't cares.*/
  uint32 mask;
} SYSTEMDB_OP_QUEUE_ELEM;

/*  Bit operation Macros on elem type */
#define OP_TYPE_NUM_OPS_MASK 0xF
#define OP_TYPE_NUM_OPS_OFFSET 16
#define GET_OP_TYPE_NUM_OPS( TYPE) \
  (((TYPE) >> OP_TYPE_NUM_OPS_OFFSET) & OP_TYPE_NUM_OPS_MASK)
#define SET_OP_TYPE_NUM_OPS(TYPE, VAL) \
  (TYPE) = (((TYPE) & (~(OP_TYPE_NUM_OPS_MASK << OP_TYPE_NUM_OPS_OFFSET))) | \
    (((VAL) & OP_TYPE_NUM_OPS_MASK) << OP_TYPE_NUM_OPS_OFFSET))
    
#define OP_TYPE_COMMAND_ID_MASK 0xFFFFUL
#define OP_TYPE_COMMAND_ID_OFFSET 16
#define GET_OP_TYPE_COMMAND_ID( TYPE ) \
  (((TYPE) >> OP_TYPE_COMMAND_ID_OFFSET) & OP_TYPE_COMMAND_ID_MASK)
#define SET_OP_TYPE_COMMAND_ID(TYPE, VAL) \
  (TYPE) = (((TYPE) & (~(OP_TYPE_COMMAND_ID_MASK << OP_TYPE_COMMAND_ID_OFFSET))) | \
    (((VAL) & OP_TYPE_COMMAND_ID_MASK) << OP_TYPE_COMMAND_ID_OFFSET))
    
#define OP_TYPE_RW_MASK 0x1
#define OP_TYPE_RW_OFFSET 15
#define GET_OP_TYPE_RW( TYPE ) \
  (((TYPE) >> OP_TYPE_RW_OFFSET) & OP_TYPE_RW_MASK)
#define SET_OP_TYPE_RW(TYPE, VAL) \
  (TYPE) = (((TYPE) & (~(OP_TYPE_RW_MASK << OP_TYPE_RW_OFFSET))) | \
    (((VAL) & OP_TYPE_RW_MASK) << OP_TYPE_RW_OFFSET))
    
#define OP_TYPE_ADDR_TYPE_MASK 0xF
#define OP_TYPE_ADDR_TYPE_OFFSET 11
#define GET_OP_TYPE_ADDR_TYPE(TYPE) \
  (((TYPE) >> OP_TYPE_ADDR_TYPE_OFFSET) & OP_TYPE_ADDR_TYPE_MASK)
#define SET_OP_TYPE_ADDR_TYPE(TYPE, VAL) \
  (TYPE) = (((TYPE) & (~(OP_TYPE_ADDR_TYPE_MASK << OP_TYPE_ADDR_TYPE_OFFSET))) | \
    (((VAL) & OP_TYPE_ADDR_TYPE_MASK) << OP_TYPE_ADDR_TYPE_OFFSET))
    
#define OP_TYPE_EVENT_TYPE_MASK 0x7FF
#define OP_TYPE_EVENT_TYPE_OFFSET 0
#define GET_OP_TYPE_EVENT_TYPE(TYPE) \
  (((TYPE) >> OP_TYPE_EVENT_TYPE_OFFSET) & OP_TYPE_EVENT_TYPE_MASK)
#define SET_OP_TYPE_EVENT_TYPE(TYPE, VAL) \
  (TYPE) = (((TYPE) & (~(OP_TYPE_EVENT_TYPE_MASK << OP_TYPE_EVENT_TYPE_OFFSET))) | \
    (((VAL) & OP_TYPE_EVENT_TYPE_MASK) << OP_TYPE_EVENT_TYPE_OFFSET))

#endif
