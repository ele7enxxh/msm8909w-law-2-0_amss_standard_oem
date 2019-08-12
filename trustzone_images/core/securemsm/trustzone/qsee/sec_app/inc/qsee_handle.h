#ifndef __QSEE_HANDLE_H
#define __QSEE_HANDLE_H

/**
@file message.h
@brief Type definitions and constants for qsee message passing.  
*/

/*===========================================================================
   Copyright (c) 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header:
  $DateTime:
  $Author:

when       who      what, where, why
--------   ---      ------------------------------------
03/02/13   nbeier      Initial version.

===========================================================================*/
#include "lkthread.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#define MAX_HANDLES 512 
#define HANDLE_ID_MASK 0xfff /* must be large enough to contain NUM_HANDLES */

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/
typedef enum {
  SVC_NONE,
  SVC_CIPHER_CTX,
  SVC_HASH_CTX,
  SVC_MAX,
} svc_id_t;


struct handle {
  uint32 app_id;  /* Reference to he application that created this handle */
  svc_id_t svc_id;  /* The service that is using this handle */
  uint32_t handle;  /* The handle value used by the application */
  void *ptr;    /* The pointer to the service context */
} ;

/*---------------------------------------------------------------------------
 *  Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/* Must be called before using qsee_handle.  Allocates memory for the table. */
void qsee_handle_init_table(void);

/* Create a handle for a given service and assign to the app 
    returns the handle value, or 0 if error. */
uint32 qsee_handle_create(void *ctx, svc_id_t svc, uint32 app);

/* Frees the slot associated with the handle, returns SUCCESS or FAILURE */
int qsee_handle_destroy(uint32 handle,  svc_id_t svc, uint32 app);

/* returns the pointer associated with a handle */
void* qsee_handle_h2p(uint32 handle,  svc_id_t svc, uint32 app);

#endif
