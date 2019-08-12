#ifndef QHSUSB_BOOT_DLOAD_H
#define QHSUSB_BOOT_DLOAD_H

#include "comdef.h"
#include "msmhwio.h"
#include "msmhwioreg.h"


/*==============================================================================
  Forward Declarations
==============================================================================*/

/*==============================================================================
  Constants
==============================================================================*/

/*==============================================================================
  Macros
==============================================================================*/

/*==============================================================================
  Typedefs
==============================================================================*/

/* Version number of the IRAM/shared segment/IMEM data */
#ifdef FEATURE_GOBI
  /* Added new variable "vendor_id" for Gobi */
  #define QHSUSB_BOOT_DLOAD_VERSION_NUM 4
#else 
  #define QHSUSB_BOOT_DLOAD_VERSION_NUM 3
#endif /* FEATURE_GOBI */

/* Macro DLOAD_QPST_COOKIE_SIZE is defined in the builds file*/
#ifndef DLOAD_QPST_COOKIE_SIZE
  /* Maximum size of QPST Cookie Data */
  #define DLOAD_QPST_COOKIE_SIZE    8
#endif /* DLOAD_QPST_COOKIE_SIZE */

/** Maximum length for the serial number.
*/
#define QHSUSB_SERIAL_NUMBER_LEN 128

/** A constant serial number to be used when IMEI/ESN is not available.
*/
#define QHSUSB_DEFAULT_SERIAL_NUMBER "1234567890ABCDEF"

/** Minimum length allowed for the serial number according to the USB Mass
    Storage Class Specification.
*/
#define QHSUSB_SERIAL_NUMBER_MIN_LEN 12

/* Magic numbers for notifying SBL3 to boot if Product ID/Serial Number is set into download mode. */
#define PID_MAGIC_NUMBER           0x71432909
#define SERIAL_NUMBER_MAGIC_NUMBER 0x61945374

#define QHSUSB_PRODUCT_ID_INVALID  0xFFFF

/* Structure to define if Product ID/Serial Number is set before entering download mode. */ 
typedef struct
{
  uint32 magic_1;
  uint32 magic_2;
} magic_cookie_type;


/* Structure to define QPST cookie to identify phone in download mode. */ 
typedef struct
{
  uint8 qpst_cookie_length;
  byte qpst_cookie_data[DLOAD_QPST_COOKIE_SIZE];
} dload_qpst_cookie_type;

/* Structure for passing information required for the SW download between HLOS, 
** DLOAD and ARMPRG  
** Use dload_info_free array within structure for all the futue additions to structure.
** This structure consumes 140 bytes out of 200 Bytes allocated for the USB IRAM parition 
*/
typedef struct
{
 /* The product ID used by HS-USB before going into download mode.
  ** It is used in download mode (DLOAD and ARMPRG), so that the phone
  ** does not switch com ports.
  */
 /* The Product ID is 2 bytes only. The users will have to typecast for the same*/ 
 uint32                          qhsusb_product_id; 
  /* The Serial Number used by HS-USB before going into download mode.
  ** It is used in download mode (DLOAD and ARMPRG), so that the phone
  ** does not switch com ports.
  */
  char                           serial_number[QHSUSB_SERIAL_NUMBER_LEN];
  magic_cookie_type              pid_serial_num_cookie;
}
qhsusb_dload_info_type;

/*==============================================================================
  PUBLIC FUNCTION DECLARATIONS FOR MODULE
==============================================================================*/

/*==============================================================================

FUNCTION          QHSUSB_BOOT_DLOAD_GET_QPST_COOKIE

DESCRIPTION
Provide the QPST Cookie Length and QPST Cookie Data to boot

DEPENDENCIES
None.

RETURN VALUE
TRUE if buffer pointer is valid, QPST cookie is returned
FALSE if buffer pointer is not valid/unallocated
FALSE if the feature FEATURE_QPST_COOKIE is not defined in builds file

SIDE EFFECTS
None
==============================================================================*/
boolean qhsusb_boot_dload_get_qpst_cookie (dload_qpst_cookie_type *dload_qpst_cookie);

/*==============================================================================

FUNCTION          QHSUSB_BOOT_DLOAD_MARK_DLOAD_INFO_INVALID

DESCRIPTION
Mark that the DLOAD related data is invalid

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS

==============================================================================*/
void qhsusb_boot_dload_mark_dload_info_invalid(qhsusb_dload_info_type* dload_info_ptr);

/*==============================================================================

FUNCTION          QHSUSB_BOOT_DLOAD_IS_DLOAD_INFO_PID_VALID

DESCRIPTION
Checks if the DLOAD related PID is valid

DEPENDENCIES
None.

RETURN VALUE
TRUE if the PID is valid, FALSE if not

SIDE EFFECTS

==============================================================================*/
boolean qhsusb_boot_dload_is_dload_info_pid_valid(const qhsusb_dload_info_type* dload_info_ptr);

/*==============================================================================

FUNCTION          QHSUSB_BOOT_DLOAD_IS_DLOAD_INFO_SERIAL_NUM_VALID

DESCRIPTION
Checks if the DLOAD related Serial Number is valid

DEPENDENCIES
None.

RETURN VALUE
TRUE if the Serial Number is valid, FALSE if not

SIDE EFFECTS

==============================================================================*/
boolean qhsusb_boot_dload_is_dload_info_serial_num_valid(const qhsusb_dload_info_type* dload_info_ptr);

/*==============================================================================

FUNCTION          QHSUSB_BOOT_DLOAD_GET_HSU_DLOAD_INFO_PTR

DESCRIPTION
Get a pointer to the structure that holds information needed for enumeration
in download mode. This information is written in HLOS, before going into 
download mode, and read in download mode.

DEPENDENCIES
None.

RETURN VALUE
Return the address of the memory to hold the SW download data

SIDE EFFECTS

==============================================================================*/
qhsusb_dload_info_type* qhsusb_boot_dload_get_dload_info_ptr(void);


#endif /*QHSUSB_BOOT_DLOAD_H */
