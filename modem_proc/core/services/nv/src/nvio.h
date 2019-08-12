#ifndef NVIO_H
#define NVIO_H
/*==========================================================================

  N V   I T E M    M A N A G E R    I/O   S E R V I C E S  
                 H E A D E R    F I L E

DESCRIPTION

  This header file contains all the definitions necessary for to interface
  with the Non Volatile Memory Read / Write Services, in order to read /write
  NVM items using EFS.

Copyright (c)  2005-2009, 2013, 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

  
when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/06/14   sri     ESN/MEID over write support via SSM
07/27/12   sri     Added new enum member in cmd_req_enum
09/25/09   pc      Dual SIM/Dual Standby support for LCU
05/27/09   sri     Added proto types to avoid warnings
04/20/05   pc      Creation of NV2
   

===========================================================================*/


/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include <stddef.h>
#include <stdio.h>
#include "comdef.h"
#include "nv_v.h"
#include "nvi.h"
#include "nvim.h"
#include "fs_public.h"

#ifdef __cplusplus
extern "C" {
#endif


/*===========================================================================
 
                         DATA DECLARATIONS

===========================================================================*/

typedef enum {
  NV_CMD_REQ_FROM_MODEM,
  NV_CMD_REQ_FROM_DIAG,
  NV_CMD_REQ_FROM_APPS,
  NV_CMD_REQ_INVALID
} nv_cmd_req_enum_type;



/*===========================================================================

                       FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION NVIO_READ_ITEM

DESCRIPTION
  Reads an NV item from NV database.

RETURN VALUE
  NV_DONE_S if the read operation is successed.
  NV_FAIL_S or NV_BADPARM_S if the read operation is fail.

===========================================================================*/
nv_stat_enum_type 
nvio_read_item (
 nv_items_enum_type item,   /* name of the item */
 byte index,
 void * nv_data_ptr,    /* data buffer to copy to */
 uint32 item_size,
 uint16 context
);


/*===========================================================================

FUNCTION NVIO_READ

DESCRIPTION
  This procedure processes NVM read requests.  It checks to make sure
  that a valid item has been requested, it reads the NVM item using EFS
  services and it performs the translation from internal NVM format to
  external format. 
  
DEPENDENCIES
  None.

RETURN VALUE
  Status of read operation.

SIDE EFFECTS
  None.

===========================================================================*/

extern nv_stat_enum_type 
nvio_read
(
  nv_cmd_ext_type  *cmd_ext_ptr,          /* Command block */
  uint32           cmd_req_from    /* cmd requested from local/remote proc */
);


/*===========================================================================

FUNCTION NVIO_READ_PRL_ITEM

DESCRIPTION
  This function reads a PRL item from NVM directory

DEPENDENCIES
  None.

RETURN VALUE
  NV_DONE_S if it worked
  NV_FAIL_S if it did not work.

SIDE EFFECTS

===========================================================================*/
nv_stat_enum_type 
nvio_read_prl_item (
  int num,
  void * data_ptr,
  int item_size
);

/*===========================================================================

FUNCTION NVIO_WRITE_ITEM

DESCRIPTION
  Writes an NV fixed item to NV database.

RETURN VALUE
  NV_DONE_S if the write operation is successed.
  NV_FAIL_S if the write operation is fail.

===========================================================================*/
nv_stat_enum_type 
nvio_write_item (
 nv_items_enum_type item,        /* name of the item */
 byte index,
 void*    nv_data_ptr,   /* data buffer to copy from */
 uint32 item_size,
 uint16 context
);


/*===========================================================================

FUNCTION NVIO_WRITE

DESCRIPTION
  This function processes NVM write requests.  The function checks
  to make sure that a valid item has been requested, it performs any
  translations necessary from external format to internal NVM format
  and writes the item using EFS write services.

DEPENDENCIES
  None.

RETURN VALUE
  Status of write operation.

SIDE EFFECTS
  None.

===========================================================================*/

extern nv_stat_enum_type 
nvio_write (
  nv_cmd_ext_type  *cmd_ext_ptr,          /* Command block */
  uint32           cmd_req_from,   /* cmd requested from local/remote proc */
  uint32           ssm_perm       /* ssm premissions*/
);


/*===========================================================================

FUNCTION NVR_TRANSLATE_TO_EXTERNAL

DESCRIPTION
  This function translates a digit from internal 4 bit NVM storage
  to external 8 bit ASCII representation.

DEPENDENCIES
  None.

RETURN VALUE
  The 8 bit ASCII digit value.

SIDE EFFECTS
  None.

===========================================================================*/

LOCAL byte 
nvr_translate_to_external ( 
  byte   digit    /* Internal digit representation */
);


/*===========================================================================

FUNCTION NVIO_READ_DIAL

DESCRIPTION
  This procedure processes read requests for speed dial numbers.  It reads
  the NV item and it performs necessary translations and copy from internal 
  format to external format.

DEPENDENCIES
  None.

RETURN VALUE
  Status of the read request.

SIDE EFFECTS
  None.

===========================================================================*/

LOCAL nv_stat_enum_type 
nvio_read_dial (
  nv_items_enum_type    item,         /* Item to access */
  byte                  index_range,  /* Maximum allowed range for index */
  byte                  index,        /* speed dial # */
  nv_dial_type          *dial_ptr,    /* Pointer to write source buffer */
  byte                  item_context  /* Context for NV path */
);


/*===========================================================================

FUNCTION NVIO_READ_STDIAL

DESCRIPTION
  This procedure processes read requests for speed dial numbers.  It reads
  the NV item and it performs necessary translations and copy from internal 
  format to external format.

DEPENDENCIES
  None.

RETURN VALUE
  Status of the read request.

SIDE EFFECTS
  None.

===========================================================================*/

LOCAL nv_stat_enum_type 
nvio_read_stdial (
  nv_items_enum_type    item,        /* Item to access */
  byte                  index_range, /* Maximum allowed range for index */
  byte                  index,       /* speed dial # */
  nv_stdial_type        *dial_ptr,   /* Pointer to write source buffer */
  byte                  item_context /* Context for NV path */
);


/*===========================================================================

FUNCTION NVIO_READ_SMS

DESCRIPTION
  This function reads an sms depending upon the address (index) of sms 
  passed into it via the cmd_ptr.

DEPENDENCIES
  None.

RETURN VALUE
  NV_DONE_S    if it worked
  NV_BADPARM_S if requested bytes are out of range for NVM

SIDE EFFECTS
  None

===========================================================================*/

LOCAL nv_stat_enum_type 
nvio_read_sms (
  nv_cmd_ext_type  *cmd_ext_ptr          /* Command block */
);


#ifdef FEATURE_GWSMS
/*===========================================================================

FUNCTION NVIO_READ_SMS_GW

DESCRIPTION
  This function reads from an sms_gw location

DEPENDENCIES
  None.

RETURN VALUE
  NV_DONE_S    if it worked
  NV_BADPARM_S if requested bytes are out of range for NVM

SIDE EFFECTS
  None

===========================================================================*/

LOCAL nv_stat_enum_type 
nvio_read_sms_gw (
  nv_cmd_ext_type  *cmd_ext_ptr          /* Command block */
);
#endif


#ifdef NV_FEATURE_PRL_ITEMS
/*===========================================================================

FUNCTION NVIO_READ_ROAMING_LIST

DESCRIPTION
  This function reads the roaming list. 

DEPENDENCIES
  None.

RETURN VALUE
  NV_DONE_S    if it worked
  NV_BADPARM_S if requested bytes are out of range for NVM

SIDE EFFECTS
  None

===========================================================================*/

LOCAL nv_stat_enum_type 
nvio_read_roaming_list (
  nv_cmd_ext_type  *cmd_ext_ptr          
);
#endif

    
/*===========================================================================

FUNCTION NVIO_WRITE_ESN

DESCRIPTION
  This function processed a write command to the NV_ESN_I item.

DEPENDENCIES
  None.

RETURN VALUE
  NV_DONE_S if it worked error status code if it failed. 

SIDE EFFECTS
  None

===========================================================================*/

LOCAL nv_stat_enum_type 
nvio_write_esn(
  nv_cmd_ext_type  *cmd_ext_ptr,    /* Command block */
  uint32            cmd_req_from,  /* cmd requested from local/remote proc */
  uint32            ssm_perm       /* ssm premissions*/
);


/*===========================================================================

FUNCTION NVIO_WRITE_ROAMING_LIST

DESCRIPTION
  This function writes the roaming list. Handled as a special case in the 
  item manager.

DEPENDENCIES
  None.

RETURN VALUE
  NV_DONE_S    if it worked
  NV_BADPARM_S if requested bytes are out of range for NVM

SIDE EFFECTS
  None

===========================================================================*/

LOCAL nv_stat_enum_type 
nvio_write_roaming_list (
  nv_cmd_ext_type  *cmd_ext_ptr          
);


/*===========================================================================

FUNCTION NVIO_WRITE_DIAL

DESCRIPTION
  This function processes write requests for speed dial number.  It performs
  translations necessary from external to internal format and then attempts
  to write the NVM item using EFS services.

DEPENDENCIES
  None.

RETURN VALUE
  Status of write operation.

SIDE EFFECTS
  None.

===========================================================================*/

LOCAL nv_stat_enum_type 
nvio_write_dial (
  nv_items_enum_type    item,             /* Item to access */
  byte                  index_range,      /* Maximum allowed range for index */
  byte                  index,            /* speed dial # */
  nv_dial_type          *dial_ptr,        /* Pointer to write source buffer */
  byte                  item_context      /* Context of item in NV */
);


/*===========================================================================

FUNCTION NVIO_WRITE_STDIAL

DESCRIPTION
  This function processes write requests for stack dial number.  It performs
  translations necessary from external to internal format and then attempts
  to write the NV item using EFS services.

DEPENDENCIES
  None.

RETURN VALUE
  Status of write operation.

SIDE EFFECTS
  None.

===========================================================================*/

LOCAL nv_stat_enum_type 
nvio_write_stdial (
  nv_items_enum_type    item,        /* Item to access */
  byte                  index_range, /* Maximum allowed range for index */
  byte                  index,       /* speed dial # */
  nv_stdial_type        *dial_ptr,   /* Pointer to external write source buffer */
  byte                  item_context /* Context of item in NV */
);


/*===========================================================================

FUNCTION NVIO_WRITE_SMS

DESCRIPTION
  This function writes to an sms EFS file.

DEPENDENCIES
  None.

RETURN VALUE
  NV_DONE_S    if it worked
  NV_BADPARM_S if requested bytes are out of range for NVM

SIDE EFFECTS
  None

===========================================================================*/

LOCAL nv_stat_enum_type 
nvio_write_sms (
  nv_cmd_ext_type         *cmd_ext_ptr       /* Command block */
);


#ifdef FEATURE_GWSMS
/*===========================================================================

FUNCTION NVIO_WRITE_SMS_GW

DESCRIPTION
  This function writes to an sms_gw EFS file.

DEPENDENCIES
  None.

RETURN VALUE
  NV_DONE_S    if it worked
  NV_BADPARM_S if requested bytes are out of range for NVM

SIDE EFFECTS
  None

===========================================================================*/

LOCAL nv_stat_enum_type 
nvio_write_sms_gw (
  nv_cmd_ext_type         *cmd_ext_ptr       /* Command block */
);
#endif


/*===========================================================================

FUNCTION NVIO_WRITE_DIR_NUMBER

DESCRIPTION
  This function performs the "cross-update" between NV_DIR_NUMBER_I and
  NV_DIR_NUMBER_PCS_I.  That is, a write to either item is translated
  and copied to the other item as well.  THIS IS A TEMPORARY FUNCTION
  ONLY USED TO EASE THE TRANSITION FROM THE OLD STRUCTURE TO THE NEW.

DEPENDENCIES
  None.

RETURN VALUE
  NV_DONE_S    if it worked
  other        if a called function returns a failure status

SIDE EFFECTS
  None

===========================================================================*/

LOCAL nv_stat_enum_type 
nvio_write_dir_number (
  nv_cmd_ext_type         *cmd_ext_ptr       /* Command block */
);


#ifdef FEATURE_RFCAL_VERSION
/*===========================================================================

FUNCTION NVIO_WRITE_RF_CAL_DATE

DESCRIPTION
  This function processed a write command to the NV_RF_CAL_DATE_I item.

DEPENDENCIES
  None.

RETURN VALUE
  NV_DONE_S if it worked error status code if it failed. 

SIDE EFFECTS
  None

===========================================================================*/

LOCAL nv_stat_enum_type 
nvio_write_rf_cal_date (
  nv_cmd_ext_type         *cmd_ext_ptr       /* Command block */
);
#endif


#ifdef FEATURE_RFCAL_VERSION
/*===========================================================================

FUNCTION NVIO_WRITE_RF_CAL_DAT_FILE

DESCRIPTION
  This function processed a write command to the NV_RF_CAL_DAT_FILE_I item.

DEPENDENCIES
  None.

RETURN VALUE
  NV_DONE_S if it worked error status code if it failed. 

SIDE EFFECTS
  None

===========================================================================*/

LOCAL nv_stat_enum_type 
nvio_write_rf_cal_dat_file (
  nv_cmd_ext_type         *cmd_ext_ptr       /* Command block */
);
#endif


/*===========================================================================
 
FUNCTION NVIO_WRITE_MEID
 
DESCRIPTION
  This function processed a write command to the NV_MEID_I item.
 
DEPENDENCIES
  None.
 
RETURN VALUE
  NV_DONE_S if it worked error status code if it failed. 
 
SIDE EFFECTS
  None
 
===========================================================================*/
 
LOCAL nv_stat_enum_type 
nvio_write_meid (
  nv_cmd_ext_type         *cmd_ext_ptr,    /* Command block */
  uint32            cmd_req_from,  /* cmd requested from local/remote proc */
  uint32            ssm_perm       /* ssm premissions*/
);

    
/*===========================================================================

FUNCTION NVIO_WRITE_IMEI

DESCRIPTION
  This function processed a write command to the NV_UE_IMEI_I item.

DEPENDENCIES
  None.

RETURN VALUE
  NV_DONE_S if it worked error status code if it failed. 

SIDE EFFECTS
  None

===========================================================================*/

LOCAL nv_stat_enum_type 
nvio_write_imei
(
  nv_cmd_ext_type         *cmd_ext_ptr,       /* Command block */
  uint32           cmd_req_from,   /* cmd requested from local/remote proc */
  uint32           ssm_perm       /* ssm premissions*/
);


/*===========================================================================

FUNCTION NVIO_WRITE_RENTAL_TIMER

DESCRIPTION
  This function writes an arbitrary value to the rental timer item.  
  Since this item is expected to be updated with very high frequency,
  it is stored in a way that minimizes EEPROM cell wear.  This unusual
  storage format calls for a special routine to write and read the
  value.
  
  The NV "increment" command would normally be used to update this item;
  this routine should only be used to initialize the item to some desired
  specific value.
  
  
DEPENDENCIES
  None.

RETURN VALUE
  NV_DONE_S    if it worked
  other        if a called function returns a failure status

SIDE EFFECTS
  None

===========================================================================*/
#ifdef NV_FEATURE_RENTAL_ITEMS

nv_stat_enum_type 
nvio_write_rental_timer (
  nv_cmd_ext_type  *cmd_ext_ptr          /* Command block */
);
#endif


/*===========================================================================

FUNCTION NVIMW_INCREMENT

DESCRIPTION
  This function processes NVM item "increment" requests.  Currently, only
  the rental timer item can use this operation.  This special processing
  is needed because the rental timer item is an unusually high wear item,
  expected to be written very frequently.

DEPENDENCIES
  None.

RETURN VALUE
  Status of the increment operation.

SIDE EFFECTS
  None.

===========================================================================*/

nv_stat_enum_type nvimw_increment
(
  nv_cmd_type  *cmd_ptr          /* Command block */
);
/*===========================================================================

FUNCTION NVIO_WRITE_PRL_ITEM

DESCRIPTION
  This function writes a PRL item into NVM directory

DEPENDENCIES
  None.

RETURN VALUE
  NV_DONE_S if it worked
  NV_FAIL_S if it did not work.

SIDE EFFECTS

===========================================================================*/
nv_stat_enum_type 
nvio_write_prl_item (
  int num,
  void * data_ptr,
  int item_size
);


/*===========================================================================

FUNCTION NVIM_INCREMENT

DESCRIPTION
  This function processes NVM item "increment" requests.  Currently, only
  the rental timer item can use this operation.  This special processing
  is needed because the rental timer item is an unusually high wear item,
  expected to be written very frequently.

DEPENDENCIES
  None.

RETURN VALUE
  Status of the increment operation.

SIDE EFFECTS
  None.

===========================================================================*/

nv_stat_enum_type 
nvim_increment (
  nv_cmd_ext_type  *cmd_ext_ptr, /* Command block */
  uint32           cmd_req_from  /* command requested from local/remote proc */
);

#ifdef __cplusplus
}
#endif

#endif /* NVIMW.H */

