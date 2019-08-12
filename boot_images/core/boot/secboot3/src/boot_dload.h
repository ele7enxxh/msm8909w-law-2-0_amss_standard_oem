#ifndef BOOT_DLOAD_H
#define BOOT_DLOAD_H

/*===========================================================================

                        Boot Downloader
                          Header File 

DESCRIPTION
 Contains common defintion used by boot downloader.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/secboot3/src/boot_dload.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/08/14   ck      Added SBL_BOOT_MISC_DETECT_MASK to identify which bits SBL
                   owns in BOOT_MISC_DETECT register.
01/08/14   ck      Added SBL_DLOAD_MODE_BIT_MASK which is new download cookie
01/06/14   ck      Removed SBL_DLOAD_MAGIC_NUM_2 as Bear family uses 32 bit
                   register to hold dload magic number.
11/16/12   dh      Add boot_dload_set_cookie
09/29/11   dh      Add boot_dload_transition_enter_sahara
06/01/11   aus     Initial revision.

============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "comdef.h"
#include "boot_sbl_shared.h"


/*===========================================================================

                     PUBLIC TYPE DEFINITIONS

===========================================================================*/

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/* In Bear family SBL owns four bits in the BOOT_MISC_DETECT register.
   In this register one bit tells SBL to enter download mode.  The following
   mask and bit field definition dictate what bits SBL owns and which one
   enables download mode. */
#define SBL_BOOT_MISC_DETECT_MASK   0x000000F0
#define SBL_DLOAD_MODE_BIT_MASK     0x00000010

/*!
 *  This function pointer is defined in each SBL* Bootloader to handle SBL-specific
 *  requirements to enter a download routine. It serves as an entry point of dload mode
 */
extern void (*sbl_dload_entry)(void);

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION  BOOT_DLOAD_ENTRY

DESCRIPTION
  Determines if identifier is present in IRAM to tell OEMSBL to enter
  the boot downloader, instead on continuing the normal boot process.
  
DEPENDENCIES
  Data in IRAM is retained across a reset.
  
RETURN VALUE
  TRUE indicates downloader should be entered.

SIDE EFFECTS
  None

===========================================================================*/
boolean boot_dload_entry( void );

/*===========================================================================

FUNCTION  BOOT_DLOAD_IS_DLOAD_MODE_SET

DESCRIPTION
This function shall return if the dload cookie is set or not for the
phone to go in download mode.

DEPENDENCIES
  Data in IRAM is retained across a reset.
  
RETURN VALUE
  TRUE indicates cookies are set
  FALSE cookies are not set
  
SIDE EFFECTS
  None

===========================================================================*/
boolean boot_dload_is_dload_mode_set( void );

/*=============================================================================

**  Function :  boot_dload_transition_pbl_forced_dload

** ==========================================================================
*/
/*!
* 
* @brief
*   This function sets the magic numbers for PBL to detect and enter forced
*   download mode.  It then calls the target specific function to trigger a
*   system reset.
* 
* @par Dependencies
*   None
*   
* @retval
*   None
* 
* @par Side Effects
*   Set's PBL magic numbers to enter forced download, resets target, never to
*   return.
* 
*/
void boot_dload_transition_pbl_forced_dload( void );


/*===========================================================================

**  Function :  boot_init_for_dload

** ==========================================================================
*/
/*!
* 
* @brief
*   Wrapper function that calls smem_boot_debug_init().
*  
* @param[in] bl_shared_data Pointer to shared data structure
*  
* @par Dependencies
*   None
*   
* @retval
*   None                                                               
* 
* @par Side Effects
*   None
* 
*/
void boot_init_for_dload( bl_shared_data_type *bl_shared_data );


/*===========================================================================

**  Function :  boot_dload_check

** ==========================================================================
*/
/*!
* 
* @brief
*   This function will check to see if the downloader should be entered
*   for QPST download, and enter the downloader if directed to.
*  
* @param[in] bl_shared_data Pointer to the shared data 
*
* @par Dependencies
*   Download ID must be present in IRAM if downloader is to be entered.
* 
* @retval
*   None
* 
* @par Side Effects
*   Boot may be halted and QPST downloader entered.
* 
*/
void boot_dload_check( bl_shared_data_type *bl_shared_data );


/*===========================================================================

**  Function :  boot_dload_transition_enter_sahara

** ==========================================================================
*/
/*!
* 
* @brief
*   This function is implemented in each SBL to enter sahara dload mode
*
* @par Dependencies
*   None
*   
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void boot_dload_transition_enter_sahara(void);


/*===========================================================================
**  Function :  boot_dload_set_cookie
** ==========================================================================
*/
/*!
* 
* @brief
*   Set the SBL dload mode cookie
**        
* @par Dependencies
*  None
*   
*/
void boot_dload_set_cookie(void);

#endif /* BOOT_DLOAD_H */
