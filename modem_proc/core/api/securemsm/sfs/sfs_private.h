#ifndef SFS_PRIVATE_H
#define SFS_PRIVATE_H

/** 
@file sfs_private.h 
@brief Secure File Partition API.
This file contains the definitions of the constants, data 
structures, and interfaces that comprise the Secure Flash 
Partition 1.0 API. 
 */

/*=============================================================================
              Copyright (c) 2000-2010 Qualcomm Technologies Incorporated.
                      All Rights Reserved.
              Qualcomm Confidential and Proprietary

=============================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.mpss/3.5.c12.3/api/securemsm/sfs/sfs_private.h#1 $ 
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/06/10   sd      [TechPubs] Edited Doxygen markup and comemnts.
06/03/10   sm      added doxygen comments, removed unecessary SFS_PRIV_KEY_SIZE
03/31/10   ejt     added sfs_priv_meid_read and sfs_priv_meid_write APIs 
                   removed sfs_priv_file_read and sfs_priv_file_write APIs 
08/28/09   ejt     removed #include "customer.h" and FEATURE flags for CMI compliance
03/22/09   sm      updated api documentation and removed a redundant function
05/07/07   avm     removed FEATURE_SEC_SFS_SECURE_PARTITION featurization
12/05/06   df      converted to cpp, externed for c access
10/12/06   df      added sfs_priv_errno_enum_type
04/04/06   df      new hdr config api
03/22/06   df      numerous checks added
                   changes to support seed sector on secure partition
07/22/05   df      FEATURE_SEC_SFS
03/20/05   df      changed priv key size macro from 16 to 256
=============================================================================*/

/**
@addtogroup SFP
@{
 */

#include "comdef.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
Identifies the error codes.
 */
typedef enum
{
  SFS_PRIV_SUCCESS            =  0,    /**< Operation was successful. */
  SFS_PRIV_FAILURE            =  1,    /**< Operation failed; the error is unknown. */
  SFS_PRIV_READONLY           =  2,    /**< Operation failed; the item is read-only. */
  SFS_PRIV_INACTIVE           =  3,    /**< Operation failed; the item is not provisioned. */
  SFS_PRIV_BAD_PARAM          =  4     /**< Operation failed; a bad parameter was passed. */
} sfs_priv_errno_enum_type;

/**
Identifies generic files. 

SFS_PRIV_FILE_1 to SFS_PRIV_FILE_4 are available for writing.
SFS_PRIV_FILE_6 to SFF_PRIV_FILE_MAX are reserved for SFP 
internal use. 
 */
typedef enum
{
  SFS_PRIV_FILE_1            =  0,    /**< &nbsp; */
  SFS_PRIV_FILE_2,                    /**< &nbsp; */
  SFS_PRIV_FILE_3,                    /**< &nbsp; */
  SFS_PRIV_FILE_4,                    /**< &nbsp; */
  SFS_PRIV_FILE_5,                    /**< &nbsp; */
  SFS_PRIV_FILE_6,                    /**< &nbsp; */
  SFS_PRIV_FILE_7,                    /**< &nbsp; */
  SFS_PRIV_FILE_8,                    /**< &nbsp; */
  SFS_PRIV_FILE_9,                    /**< &nbsp; */
  SFS_PRIV_FILE_10,                   /**< &nbsp; */
  SFS_PRIV_FILE_11,                   /**< &nbsp; */
  SFS_PRIV_FILE_12,                   /**< &nbsp; */
  SFS_PRIV_FILE_13,                   /**< &nbsp; */
  SFS_PRIV_FILE_14,                   /**< &nbsp; */
  SFS_PRIV_FILE_15,                   /**< &nbsp; */
  SFS_PRIV_FILE_16,                   /**< &nbsp; */
  SFS_PRIV_FILE_17,                   /**< &nbsp; */
  SFS_PRIV_FILE_18,                   /**< &nbsp; */
  SFS_PRIV_FILE_19,                   /**< &nbsp; */
  SFS_PRIV_FILE_20,                   /**< &nbsp; */
  SFS_PRIV_FILE_21,                   /**< &nbsp; */
  SFS_PRIV_FILE_22,                   /**< &nbsp; */
  SFS_PRIV_FILE_23,                   /**< &nbsp; */
  SFS_PRIV_FILE_24,                   /**< &nbsp; */
  SFS_PRIV_FILE_25,                   /**< &nbsp; */
  SFS_PRIV_FILE_26,                   /**< &nbsp; */
  SFS_PRIV_FILE_27,                   /**< &nbsp; */
  SFS_PRIV_FILE_28,                   /**< &nbsp; */
  SFS_PRIV_FILE_29,                   /**< &nbsp; */
  SFS_PRIV_FILE_30,                   /**< &nbsp; */
  SFS_PRIV_FILE_31,                   /**< &nbsp; */
  SFS_PRIV_FILE_32,                   /**< &nbsp; */
  SFS_PRIV_FILE_33,                   /**< &nbsp; */
  SFS_PRIV_FILE_34,                   /**< &nbsp; */
  SFS_PRIV_FILE_35,                   /**< &nbsp; */
  SFS_PRIV_FILE_36,                   /**< &nbsp; */
  SFS_PRIV_FILE_37,                   /**< &nbsp; */
  SFS_PRIV_FILE_38,                   /**< &nbsp; */
  SFS_PRIV_FILE_39,                   /**< &nbsp; */
  SFS_PRIV_FILE_40,                   /**< &nbsp; */
  SFS_PRIV_FILE_41,                   /**< &nbsp; */
  SFS_PRIV_FILE_42,                   /**< &nbsp; */
  SFS_PRIV_FILE_43,                   /**< &nbsp; */
  SFS_PRIV_FILE_44,                   /**< &nbsp; */
  SFS_PRIV_FILE_45,                   /**< &nbsp; */
  SFS_PRIV_FILE_MAX                   /**< &nbsp; */
} sfs_priv_file_enum_type;

/**
Writes private data to the simloc file in the SFS or SFP. 
 
Callers are to call sfs_oem_priv_data_loc_get() to see if they
can use this API.
 
@param[in] buf  Buffer holding the data to write.
@param[in] len  Number of bytes to write.
 
@return 
- Number of bytes written. \n
- 0 if no bytes are written or an error occurred.
 
@dependencies
None.

@sa sfs_oem_priv_data_loc_get
 */
int sfs_priv_simloc_write(uint8 *buf, int len);


/**
Reads simloc data.

Callers are to call sfs_oem_priv_data_loc_get() to see if they
can use this API.
 
@param[in,out] buf Buffer to read the data into.
@param[in] len     Number of bytes to read.
 
@return
- Number of bytes read. \n
- 0 if no bytes were read or an error occurred.
 
@dependencies
None.

@sa 
 */
int sfs_priv_simloc_read(uint8 *buf, int len);


/**
Writes the private data to the IMEI file in the SFS or SFP. 

Callers are to call sfs_oem_priv_data_loc_get() to see if they
can use this API.
 
@param[in] buf  Buffer holding the data to write.
@param[in] len  Number of bytes to write.
 
@return 
SFS_PRIV_SUCCESS   - Operation was successful. \n
SFS_PRIV_FAILURE   - Operation failed; the error is unknown. \n 
SFS_PRIV_READONLY  - Operation failed; the item is read-only. \n
SFS_PRIV_INACTIVE  - Operation failed; the item is not provisioned. \n
SFS_PRIV_BAD_PARAM - Operation failed; bad parameters were passed. 
 
@dependencies
None.
 
@sideeffects
Multiple writes can be disabled depending on the OEM implementation.
See secapi_oem_allow_imei_write().

@sa sfs_oem_priv_data_loc_get
 */
sfs_priv_errno_enum_type sfs_priv_imei_write(uint8 *buf, int len);


/**
Reads IMEI data.
 
Callers are to call sfs_oem_priv_data_loc_get() to see if they can use this API.

@param[in,out] buf Buffer to read the data into.
@param[in] len     Number of bytes to read.
 
@return 
SFS_PRIV_SUCCESS   - Operation was successful. \n                
SFS_PRIV_FAILURE   - Operation failed; the error is unknown. \n 
SFS_PRIV_READONLY  - Operation failed; the item is read-only. \n
SFS_PRIV_INACTIVE  - Operation failed; the item is not provisioned. \n
SFS_PRIV_BAD_PARAM - Operation failed; bad parameters were passed.
 
@dependencies
None.
 
@sa sfs_priv_imei_write
 */
sfs_priv_errno_enum_type sfs_priv_imei_read(uint8 *buf, int len);


/**
Writes private data to the MEID file in the SFS or SFP. 
 
Callers are to call sfs_oem_priv_data_loc_get() to see if they
can use this API.
 
@param[in] buf  Buffer holding the data to write.
@param[in] len  Number of bytes to write.
 
@return 
SFS_PRIV_SUCCESS   - Operation was successful. \n                
SFS_PRIV_FAILURE   - Operation failed; the error is unknown.  \n
SFS_PRIV_READONLY  - Operation failed; the item is read-only. \n
SFS_PRIV_INACTIVE  - Operation failed; the item is not provisioned. \n
SFS_PRIV_BAD_PARAM - Operation failed, bad parameters were passed.
 
@dependencies
None.
 
@sideeffects
Multiple writes may be disabled depending on the OEM implementation.
See secapi_oem_allow_imei_write(). MEID and IMEI use the same write control logic.

@sa sfs_oem_priv_data_loc_get
 */
sfs_priv_errno_enum_type sfs_priv_meid_write(uint8 *buf, int len);


/**
Reads MEID data.
 
Callers are to call sfs_oem_priv_data_loc_get() to see if they can use this API.

@param[in,out] buf Buffer to read the data into.
@param[in] len     Number of bytes to read.

@return 
SFS_PRIV_SUCCESS   - Operation was successful.  \n               
SFS_PRIV_FAILURE   - Operation failed; the error is unknown.  \n
SFS_PRIV_READONLY  - Operation failed; the item is read-only. \n
SFS_PRIV_INACTIVE  - Operation failed; the item is not provisioned. \n 
SFS_PRIV_BAD_PARAM - Operation failed; bad parameters were passed. 
 
@dependencies
None.

@sa sfs_priv_meid_write
 */
sfs_priv_errno_enum_type sfs_priv_meid_read(uint8 *buf, int len);


/**
Writes private data to the file specified by the 
file type (sfs_priv_file_enum_type()) in the SFS or SFP.
 
Callers are to call sfs_oem_priv_data_loc_get() to see if they
can use this API.

@param[in] file File to write to, as specified by sfs_priv_file_enum_type.
@param[in] buf  Buffer holding the data to write.
@param[in] len  Number of bytes to write.

@return 
- Number of bytes written. \n
- 0 if no bytes written or an error occurred.
 
@dependencies
None.

@sa sfs_oem_priv_data_loc_get
 */
int sfs_priv_file_write(int file, uint8 *buf, int len);


/**
Reads private data from the file specified in the file parameter.
 
Callers are to call sfs_oem_priv_data_loc_get() to see if they
can use this API.  
 
@param[in] file     File to read from, as specified by sfs_priv_file_enum_type.
@param[in,out] buf  Buffer to read the data into.
@param[in] len      Number of bytes to read.

@return 
- Number of bytes read. \n
- 0 if no bytes were read or an error occurred.

@dependencies
None.

@sa sfs_priv_simloc_write
 */
int sfs_priv_file_read(int file, uint8 *buf, int len);

#ifdef __cplusplus
}
#endif

/** @} */  /* end_addtogroup SFP */
#endif /* SFS_PRIVATE_H */
