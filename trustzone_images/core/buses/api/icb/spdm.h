/**
@file spdm.h
@brief Hypervisor SPDM driver definitions

This file contains the definitions for the SPDM driver hypervisor HVC call

*/
/*===========================================================================
   Copyright (c) 2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/buses/api/icb/spdm.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $


when       who      what, where, why
--------   ---     ------------------------------------------------------------ 
10.10.14   jc       Created 
=============================================================================*/

#ifndef SPDM_H
#define SPDM_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

/*=========================================================================== 
    Preprocessor Definitions and Constants
============================================================================*/

/* SPDM Driver ioctl return values */
#define SPDM_RESULT_SUCCESS   0
#define SPDM_RESULT_FAILURE   (!0)

/* SPDM Driver ioctl codes */
#define SPDM_CMD_BASE                  0x00000000
#define SPDM_CMD_VERSION               (SPDM_CMD_BASE +  0)
#define SPDM_CMD_GET_BW_ALL            (SPDM_CMD_BASE +  1)
#define SPDM_CMD_GET_BW_SPECIFIC       (SPDM_CMD_BASE +  2)
#define SPDM_CMD_ENABLE                (SPDM_CMD_BASE +  3)
#define SPDM_CMD_DISABLE               (SPDM_CMD_BASE +  4)
#define SPDM_CMD_CFG_PORTS             (SPDM_CMD_BASE +  5)
#define SPDM_CMD_CFG_FLTR              (SPDM_CMD_BASE +  6)
#define SPDM_CMD_CFG_PL                (SPDM_CMD_BASE +  7)
#define SPDM_CMD_CFG_REJRATE_LOW       (SPDM_CMD_BASE +  8)
#define SPDM_CMD_CFG_REJRATE_MED       (SPDM_CMD_BASE +  9)
#define SPDM_CMD_CFG_REJRATE_HIGH      (SPDM_CMD_BASE + 10)
#define SPDM_CMD_CFG_RESPTIME_LOW      (SPDM_CMD_BASE + 11)
#define SPDM_CMD_CFG_RESPTIME_MED      (SPDM_CMD_BASE + 12)
#define SPDM_CMD_CFG_RESPTIME_HIGH     (SPDM_CMD_BASE + 13)
#define SPDM_CMD_CFG_CCIRESPTIME_LOW   (SPDM_CMD_BASE + 14)
#define SPDM_CMD_CFG_CCIRESPTIME_MED   (SPDM_CMD_BASE + 15)
#define SPDM_CMD_CFG_CCIRESPTIME_HIGH  (SPDM_CMD_BASE + 16)
#define SPDM_CMD_CFG_MAXCCI            (SPDM_CMD_BASE + 17)
#define SPDM_CMD_CFG_VOTES             (SPDM_CMD_BASE + 18)

/* List of supported master ports */
#define SPDM_MONITOR_CPU         0
#define SPDM_MONITOR_GPU         1
#define SPDM_MONITOR_COUNT       2

#define SPDM_CLIENT_INVALID      0xFFFFFFFF

/* Max number of performance levels */
#define SPDM_PL_COUNT            3

/* Max number of monitor ports */
#define SPDM_MONITOR_PORT_COUNT  2

/* ioctl parameter definitions */
#define SPDM_IOCTL_INPUT_COUNT   6
#define SPDM_IOCTL_OUTPUT_COUNT  3

/*=========================================================================== 
    Types
============================================================================*/

/* SPDM Driver ioctl */
typedef struct
{
   uint64 input[SPDM_IOCTL_INPUT_COUNT];
   uint64 output[SPDM_IOCTL_OUTPUT_COUNT];
} spdm_ioctl_type;

/*===========================================================================

          FUNCTIONS

============================================================================*/

/* SPDM Driver ioctl */
extern uint32 spdm_ioctl
(
   spdm_ioctl_type *ioctl_p
);

#endif
