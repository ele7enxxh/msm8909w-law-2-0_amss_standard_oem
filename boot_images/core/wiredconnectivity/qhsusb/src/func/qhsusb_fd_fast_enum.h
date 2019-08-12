#ifndef QHSUSB_FD_FAST_ENUM_H
#define QHSUSB_FD_FAST_ENUM_H
/*=======================================================================*//**
  Functional driver for fast enumeration function

  @file         qhsusb_fd_fast_enum.h
 
  @brief        Fast enumeration Functional Driver
 
  @details      
 
               Copyright (c) 2008 Qualcomm Technologies Incorporated.
               All Rights Reserved.
               Qualcomm Confidential and Proprietary
*//*========================================================================*/
/* $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/wiredconnectivity/qhsusb/src/func/qhsusb_fd_fast_enum.h#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
 * Init
 * 
 * Should be called prior to any other functions
 * 
 *  
 */
struct qhsusb_dcd_dsc_device* qhsusb_fd_fast_enum_init(void);

/**
 * Terminates connection.
 * After this function, interface is not valid
 * and its functions can't be used.
 */
void qhsusb_fd_fast_enum_shutdown(void);

#endif /* #ifndef QHSUSB_FD_FAST_ENUM_H */

