/*!
  @file
  rf_hal_bus_types.h

  @brief
  RF HAL bus data types
 
*/

/*===========================================================================
Copyright (c) 2012 - 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/api/ccs/rf_hal_bus_types.h#1 $
$DateTime: 2016/12/13 07:59:54 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
10/16/13   ra      Support CCS portability
08/28/12   jfc     Support masked ssbi writes thru access task
08/18/12   jfc     Add field to force RFFE extended format
07/06/12   jfc     Swap addr and num_bytes fields in rf_hal_bus_rffe_type struct
07/05/12   sty     Added delimiters in rf_hal_bus_resource_type 
07/02/12   jfc     Force data field to be on one word line
06/25/12   jfc     Fix comments about read and write values
05/16/12   jfc     Make the read and write values an enum
05/15/12   jfc     Add more types 
05/09/12   jfc     Fix RD and WR #define values; make rd_delay a boolean type 
05/07/12   jfc     Initial version
============================================================================*/

#ifndef RF_HAL_BUS_TYPES_H
#define RF_HAL_BUS_TYPES_H

#include "comdef.h"



/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief The numbef of bytes for extended RFFE write/reads. MIPI standard is minimum 4, maximum 16 */
#define RF_HAL_BUS_RFFE_MAX_BYTES       4

/*! @brief #defines for RFFE buses */
#define RF_HAL_BUS_RFFE_0               0
#define RF_HAL_BUS_RFFE_1               1

/*! @brief #defines for SSBI buses */
#define RF_HAL_BUS_SSBI_0               0
#define RF_HAL_BUS_SSBI_1               1
#define RF_HAL_BUS_SSBI_2               2
#define RF_HAL_BUS_SSBI_3               3
#define RF_HAL_BUS_SSBI_4               4
#define RF_HAL_BUS_SSBI_5               5

/*! @brief Read or write operations */
#define RF_HAL_BUS_WRITE_DEFINE             0
#define RF_HAL_BUS_READ_DEFINE              1
#define RF_HAL_BUS_READ_MODIFY_WRITE_DEFINE 2

/*! @brief Read or write operations */
typedef enum
{
  RF_HAL_BUS_WRITE               = RF_HAL_BUS_WRITE_DEFINE,
  RF_HAL_BUS_READ                = RF_HAL_BUS_READ_DEFINE,
  RF_HAL_BUS_READ_MODIFY_WRITE   = RF_HAL_BUS_READ_MODIFY_WRITE_DEFINE,   // masked write
} rf_hal_bus_rd_wr_type;




/*! @brief Bus resource enum

*/
typedef enum
{
  RF_HAL_BUS_RESOURCE_SSBI,    /*!< SSBI write or read  */
  RF_HAL_BUS_RESOURCE_RFFE,    /*!< GRFC write or read  */
  RF_HAL_BUS_RESOURCE_GRFC,    /*!< GRFC write */
  RF_HAL_BUS_RESOURCE_NUM,
  RF_HAL_BUS_RESOURCE_MAX=RF_HAL_BUS_RESOURCE_NUM
} rf_hal_bus_resource_type;


/*! @brief Result type for various operations 

*/
typedef enum
{
  RF_HAL_BUS_SUCCESS            = 0,      /*!< Success */
  RF_HAL_BUS_FAILURE            = 1,      /*!< Unspecified error */
  RF_HAL_BUS_BAD_PARAM          = 2,      /*!< Bad parameter passed in */
  RF_HAL_BUS_MAX_TRANS_EXCEEDED = 3,      /*!< Maximum number of transactions exceeded */
  RF_HAL_BUS_INTERNAL_ERROR     = 4       /*!< Internal error (e.g. Command interface error) */
} rf_hal_bus_result_type;


/*! @brief SBI transaction data structure

*/
typedef struct
{
  uint8 addr;                 /*!< The 8 bit SSBI address */
  uint8 data;                 /*!< The 8 bit SSBI data. Undefined if transaction is read */
  uint8 sbi_bus;              /*!< The SSBI bus number */
  rf_hal_bus_rd_wr_type rd_wr;/*!< Read or write transaction. 1 = Read, 0 = Write */
  int16 start_delta;          /*!< Start time in TBD units */
} rf_hal_bus_sbi_type;


/*! @brief GRFC transaction data structure

*/
typedef struct
{
  uint16 grfc_id;             /*!< The GRFC ID */
  int16 start_delta;          /*!< Start time in TBD units */
  uint8 start_logic;          /*!< The logical level of GRFC. 0 = Low, 1 = High */
} rf_hal_bus_grfc_type;


/*! @brief RFFE transaction data structure

*/
typedef struct
{
  rf_hal_bus_rd_wr_type rd_wr;/*!< Read or write transaction. 1 = Read, 0 = Write */
  uint8 channel;              /*!< The RFFE channel (bus) number */
  uint8 slave_id;             /*!< The RFFE slave ID */
  boolean rd_delay;           /*!< The read delay (N/A for writes) */
  boolean half_rate;          /*!< Use half rate reads (N/A for writes) */
  uint8 num_bytes;            /*!< Number of bytes to write or read */
  uint16 addr;                /*!< The RFFE register address. May be 8 or 16 bits */
  uint8 data[RF_HAL_BUS_RFFE_MAX_BYTES] ALIGN(4);   /*!< Data bytes to write or space for holding read data */
  int16 start_delta;          /*!< Start time in TBD units */
  boolean extended_cmd;       /*!< Whether this transaction uses extended command format or not (must be TRUE if addr is 16 bits or num_bytes > 1 */
} rf_hal_bus_rffe_type;


/*! @brief Bus resource script data type.

  @details A container around a RFFE, SSBI, or GRFC script. Gives the type,
  points to the data, and specifies the number of transactions
*/
typedef struct
{
  rf_hal_bus_resource_type script_type;             /*!< The type of resource used. E.g. SSBI, RFFE, GRFC */
  union
  {
    rf_hal_bus_sbi_type* sbi;
    rf_hal_bus_grfc_type* grfc;
    rf_hal_bus_rffe_type* rffe;
  } script_ptr;                                     /*!< A union of pointers to script */
  uint16 num_trans;                                 /*!< The number of transactions in the script */
} rf_hal_bus_resource_script_type;


/*! @brief Resource script type 
*/
typedef struct
{
  uint8 priority;             /*!< The priority of all transactions in the script */
  boolean preempt_error;      /*!< Abort script if it is preempted in the middle */
  boolean half_rate;          /*!< Use half-rate reads. Applicable only to RFFE */
  boolean rd_delay;           /*!< Insert delay before reading. Application only to RFFE (and SSBI?) */
  boolean rffe_force_ext;     /*!< Some devices require extended format even if address is les than 32. Set this to TRUE to force an ext command sequence */
} rf_hal_bus_resource_script_settings_type;

#endif /* RF_HAL_BUS_TYPES_H */
