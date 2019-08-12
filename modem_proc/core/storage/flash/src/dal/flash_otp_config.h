#ifndef FLASH_OTP_CONFIG_H
#define FLASH_OTP_CONFIG_H

/*=======================================================================
 * FILE:        flash_otp_config.h
 *
 * SERVICES:    OTP Configuration defines 
 *
 * DESCRIPTION: OTP configuration parameters are defined here. Also UUID params
 *              are defined in this file.
 *
 *              Copyright © 2008-2009 Qualcomm Technologies Incorporated.
 *              All Rights Reserved.
 *              QUALCOMM Proprietary/GTDR
 *=====================================================================*/

/*===================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 06/26/2009   rk      512B OTP config type added
 * 02/19/09     mh      Clean-up
 * 11/10/08     mm      Initial Revision
 *==================================================================*/

/*-----------------------------------------------------------------------
 *            Include Files
 *----------------------------------------------------------------------*/
#include "flash.h"

/* Maximum length for signature in OTP block */
#define FLASH_MAX_SIGNATURE_LEN 2

/*-----------------------------------------------------------------------
 *           Type Declarations
 *----------------------------------------------------------------------*/
/* Command type single or paired */
enum nand_cmd_type
{
  NAND_CMD_NONE,                          /* No Command */
  NAND_CMD_SINGLE,                        /* Single Cmd */
  NAND_CMD_PAIRED,                        /* Paired Cmd */
  NAND_CMD_MAX,                           /* Upper limit */
};

/* Data direction during an operation */
enum nand_data_dir
{
  NAND_DATA_NONE,                         /* No data */
  NAND_DATA_WRITE,                        /* Write to device */
  NAND_DATA_READ,                         /* Read from device */
  NAND_DATA_MAX,                          /* Upper limit */
};

/* Wait mechanism to be followed during an operation */
enum nand_wait_type
{
  NAND_WAIT_ON_Rb_NONE,                   /* No wait */
  NAND_WAIT_ON_Rb_HIGH,                   /* Ready goes HIGH - Device is Ready*/
  NAND_WAIT_ON_Rb_LOW,                    /* Ready goes LOW - Device is Busy */
  NAND_WAIT_ON_Rb_MAX,                    /* Upper limit */
};

enum nand_addr_type
{
  NAND_ADDR_NONE,                          /* No Address */
  NAND_ADDR_PAGE_NUMBER,                   /* Page number */
  NAND_ADDR_BYTE_ADDRESS                   /* Byte Array */
};

enum nand_otp_config_type
{
  FLASH_NAND_OTP_SEQUENCE_CFG1,  /* Large block NAND cfg 1 */
  FLASH_NAND_OTP_SEQUENCE_CFG2,  /* Large block NAND cfg 2 */
  FLASH_NAND_OTP_SEQUENCE_CFG3,  /* Large block NAND cfg 3 */
  FLASH_NAND_OTP_SEQUENCE_CFG4,  /* Large block NAND cfg 4 */
  FLASH_NAND_OTP_SEQUENCE_CFG5,  /* Large block NAND cfg 5 */
  FLASH_NAND_OTP_SEQUENCE_CFG6,  /* SLC Small page NAND cfg */
  FLASH_NAND_OTP_SEQUENCE_UNKNOWN
};

/* NAND Address structure */
struct nand_addr
{
  enum nand_addr_type addr_type;           /* Page or byte array */
  union
  {
    uint32 page;                            /* Page Number */
    uint8  *addr;                           /* Address in bytes */
  }u;
};

/* NAND single command structure */
struct nand_bus_cycle
{
  enum nand_cmd_type  cmd_type;              /* Command Grouping */
  uint8               cmd;                   /* Actual command */
  uint32              num_addr_cycles;       /* Address cycles following 
                                                command */
  struct nand_addr    addr;                  /* Address - page number or 
                                                byte address */
  enum nand_data_dir  data_dir;              /* Data direction, if any  */
  enum nand_wait_type wait;                  /* Wait mechanism, if any */
};

/* UUID Params */
struct nand_uuid_params
{
  uint32 uuid_len;                           /* UUID length in bytes*/
  uint32 signature_len;                      /* Signature length in bytes */
  uint8 signature [FLASH_MAX_SIGNATURE_LEN]; /* Signature bytes */
};

/* OTP Config information */
struct otp_config
{
  uint32 sequence_count;                     /* Number of OTP cmds */
  struct nand_bus_cycle *otp_sequence;   /* The OTP Sequence */
  struct nand_uuid_params  *uuid_params;     /* UUID parameters */
};

/* Flash Custom Page Operation */
struct flash_read_otp_page_op
{
  uint32 page;        /* Page to be operated on */
  void   *page_data;  /* Pointer to Buffer that used as source
                              or destination for custom operation */
  uint32 sequence_count; /* Sequence count */
  struct nand_bus_cycle *sequence;   /* Pointer to custom command sequence, if any*/
};

/*-----------------------------------------------------------------------
 *           OTP Configuration APIs
 *----------------------------------------------------------------------*/

/*
 * Gets the OTP configuration.
 */
void flash_get_otp_configs (enum nand_otp_config_type otp_sequence_cfg, 
  struct otp_config **cfg_data);

#endif  /*ifndef FLASH_OTP_CONFIG_H */
