#ifndef TZBSP_SYSCALL_PRIV_H
#define TZBSP_SYSCALL_PRIV_H

/**
   @file tzbsp_syscall_priv.h
   @brief Provide the private SYSCALL API infrastructure
*/

/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.

      The xxx_mainpage.dox file contains all descriptions that are displayed
      in the output PDF generated using Doxygen and LaTeX. To edit or update
      any of the file/group text in the PDF, edit the xxx_mainpage.dox
      file or contact Tech Pubs.
===========================================================================*/

/*===========================================================================
   Copyright (c) 2010-2012 by Qualcomm Technologies, Incorporated.
   All rights reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/include/tzbsp_syscall_priv.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $

when       who      what, where, why
--------   ---      ------------------------------------
03/21/12   pre      Initial version.

===========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "tz_syscall_pub.h"

/** DCVS service. */
#define TZ_SVC_DCVS               13

/**
   @ingroup gfx_dcvs_reset

   API for the graphics team to control the proprietary dynamic clock
   and voltage scaling routine.

   @note1hang Implementation takes place entirely in Monitor mode.

   @command_id
     0x00001403
*/
#define TZBSP_GFX_DCVS_RESET_ID                           \
  TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_IO_ACCESS, 0x03)

/**
   @ingroup gfx_dcvs_update

   API for the graphics team to control the proprietary dynamic clock
   and voltage scaling routine.

   @note1hang Implementation takes place entirely in Monitor mode.

   @command_id
     0x00001404
*/
#define TZBSP_GFX_DCVS_UPDATE_ID                          \
  TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_IO_ACCESS, 0x04)


/**
   @ingroup dcvs_create_group

   Create a DCVS "group" for linking cores.

   @param group_id Unique identifier for the group.

   @return
     \c 0                If successful.
     \c -E_NOT_ALLOWED   If DcvsInit() was not previously called.
     \c -E_NO_MEMORY     There is insufficient memory.
     \c -E_OUT_OF_RANGE  If group already exists.

   @command_id
     0x00003401

   @com_datatypes
     tz_syscall_req_s \n
     tz_syscall_rsp_s
*/
#define TZBSP_DCVS_CREATE_GROUP_ID \
        TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_DCVS, 0x1)

/**
   @ingroup dcvs_register_core

   Register a core with the DCVS system.

   @param core_id     Unique identifier for the core.
   @param group_id    Group to add the core to.
   @param core_params Params for the core.
   @param freq_entry  1d array of frequency data.

   @return
     \c 0                If successful.
     \c -E_NOT_ALLOWED   If DcvsInit() was not previously called.
     \c -E_NO_MEMORY     There is insufficient memory.
     \c -E_OUT_OF_RANGE  If group already exists.

   @command_id
     0x00003402

   @com_datatypes
     tz_syscall_req_s \n
     tz_syscall_rsp_s
*/
#define TZBSP_DCVS_REGISTER_CORE_ID \
        TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_DCVS, 0x2)

/**
   @ingroup dcvs_set_alg_params

   Set algorithm parameters for a core.

   @param core_id  Unique identifier for the core.
   @param params   Algorithm parameters.


   @return
     \c 0               If successful.
     \c -E_NOT_ALLOWED  If DcvsInit() was not previously called.
     \c -E_OUT_OF_RANGE If group already exists.

   @command_id
     0x00003403

   @com_datatypes
     tz_syscall_req_s \n
     tz_syscall_rsp_s
*/
#define TZBSP_DCVS_SET_ALG_PARAMS_ID \
        TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_DCVS, 0x3)

/**
   @ingroup dcvs_system_event

   Handles DCVS system events

   @param[in]  r0 Command ID.
   @param[in]  r1 Token.
   @param[in]  r2 Unique identifier for the core.
   @param[in]  r3 DCVS event ID.
   @param[in]  r4 Proprietary input parameter.
   @param[in]  r5 Proprietary input parameter.
   @param[out] r0 negative value on failure; zero otherwise.

   @note1hang Implementation takes place entirely in Monitor mode.

   @command_id
     0x00003404
*/
#define TZBSP_DCVS_SYSTEM_EVENT_ID \
        TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_DCVS, 0x4)

/**
   @ingroup dcvs_init

   Used to initialize the DCVS system with a working physically
   contiguous, uncached buffer.

   @param buf_addr: Physical address of a physically contiguous
                    buffer. Size is target dependent.
   @param buf_size: Size of the buffer in bytes.

   @return
     \c 0                If successful.
     \c -E_ALREADY_DONE  If already initialized.
     \c -E_OUT_OF_RANGE  If group already exists.

   @command_id
     0x00003405

   @com_datatypes
     tz_syscall_req_s \n
     tz_syscall_rsp_s
*/
#define TZBSP_DCVS_INIT_ID \
        TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_DCVS, 0x5)

/**
   @ingroup gfx_dcvs_init

   Used to initialize the Graphics DCVS.

   @param nlevels: Number of power levels.
   @param freq:  Array of frequency values.

   @return
     \c 0                If successful.
     \c -E_BAD_ADDRESS   If structure pointer/memory is invalid.
     \c -E_OUT_OF_RANGE  If power levels are out of range.
     \c -E_FAILURE       If number of power levels is zero.

   @command_id
     0x00003406

   @com_datatypes
     tz_syscall_req_s \n
     tz_syscall_rsp_s
 */
#define TZBSP_GFX_DCVS_INIT_ID                      \
  TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_DCVS, 0x06)

/*----------------------------------------------------------------------------
 * DCVS commands
 * -------------------------------------------------------------------------*/
/**
   @ingroup dcvs_create_group

   Request message structure corresponding to the message ID
   TZBSP_DCVS_CREATE_GROUP_ID.
*/
typedef struct tzbsp_dcvs_create_group_s
{
  struct tz_syscall_req_s  common_req;  /**< Common request structure. */
  uint32               group_id;    /**< Unique identifier for group
                                         to create */
} __attribute__ ((packed)) tzbsp_dcvs_create_group_t;

/**
   @ingroup dcvs_register_core

   Request message structure corresponding to the message ID
   TZBSP_DCVS_REGISTER_CORE_ID.
*/
typedef struct tzbsp_dcvs_register_core_s
{
  struct tz_syscall_req_s  common_req;   /**< Common request structure. */
  uint32               core_id;      /**< Unique identifier for the core. */
  uint32               group_id;     /**< Group to add the core to. */
  uint32*              core_params;  /**< Params for the core. */
  uint32*              freq_entry;   /**< 1d array of frequency data. */
} __attribute__ ((packed)) tzbsp_dcvs_register_core_t;

/**
   @ingroup dcvs_set_alg_params

   Request message structure corresponding to the message ID
   TZBSP_DCVS_SET_ALG_PARAMS_ID.
*/
typedef struct tzbsp_dcvs_set_alg_params_s
{
  struct tz_syscall_req_s  common_req;  /**< Common request structure. */
  uint32               core_id;     /**< Unique identifier for the core. */
  uint32*              params;      /**< Algorithm parameters. */
} __attribute__ ((packed)) tzbsp_dcvs_set_alg_params_t;

/**
   @ingroup dcvs_init

   Request message structure corresponding to the message ID
   TZBSP_DCVS_INIT_ID.
*/
typedef struct tzbsp_dcvs_init_s
{
  struct tz_syscall_req_s  common_req;  /**< Common request structure. */
  uint32               buf_addr;    /**< Physical address of a
                                         physically contiguous
                                         buffer. Size is target
                                         dependent. */
  uint32               buf_size;    /**< Size of the buffer in
                                         bytes. */
} __attribute__ ((packed)) tzbsp_dcvs_init_t;

/*----------------------------------------------------------------------------
 * Graphics DCVS structures
 * -------------------------------------------------------------------------*/

#define TZBSP_GPU_DCVS_NUMPWRLEVELS                 10

typedef struct tzbsp_gfx_dcvs_init_data_s
{
  uint32 nlevels;
  uint32 freq[TZBSP_GPU_DCVS_NUMPWRLEVELS];
} __attribute__ ((packed)) tzbsp_gfx_dcvs_init_data_t;

/**
   @ingroup gfx_dcvs_init

   Request message structure corresponding to the message ID
   TZBSP_GFX_DCVS_INIT_ID.
*/
typedef struct tzbsp_gfx_dcvs_init_req_s
{
  struct tz_syscall_req_s  common_req;  /**< Common request structure. */
  tzbsp_gfx_dcvs_init_data_t data;
} __attribute__ ((packed)) tzbsp_gfx_dcvs_init_req_t;

#endif /* TZBSP_SYSCALL_PRIV_H */
