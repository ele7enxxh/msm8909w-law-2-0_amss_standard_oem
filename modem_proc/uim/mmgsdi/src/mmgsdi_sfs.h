#ifndef MMGSDI_SFS_H
#define MMGSDI_SFS_H
/*===========================================================================


            M M G S D I   T A S K   A N D  S F S  I N T E R F A C E

                                 H E A D E R


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2013, 2015 QUALCOMM Technologies, Incorporated and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.  
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/mmgsdi/src/mmgsdi_sfs.h#1 $$ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
-------------------------------------------------------------------------------
12/25/15   bcho    Send Card and session events based on SFS data in PSM mode
06/11/13   av      Initial Revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "comdef.h"
#include "mmgsdi.h"

/* <EJECT> */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/* ------------------------------------------------------------------------*/
/*                 M M G S D I   S F S   I N T E R F A C E                 */
/*                                                                         */
/*                 F U N C T I O N        P R O T Y P E S                  */
/* ------------------------------------------------------------------------*/

/*===========================================================================
FUNCTION MMGSDI_SFS_GET_SIZE

DESCRIPTION
  This function will get the size of the sfs file.


DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type:  MMGSDI_SUCCES:    File size found
                            MMGSDI_ERROR:     SFS error
                            MMGSDI_NOT_FOUND: SFS file did not exist
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_sfs_get_size(
  uint32      *size_ptr,
  const char  *sfs_file_name
);

/*===========================================================================
FUNCTION MMGSDI_SFS_MKDIR

DESCRIPTION
  This function will make the sfs directory for perso.


DEPENDENCIES
  The parent directory has already been created.

RETURN VALUE
  mmgsdi_return_enum_type:  MMGSDI_SUCCES:    Directory was created
                            GSDI_ERROR:     Error creating directory
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_sfs_mkdir(
  const char* sfs_file_name
);

/*===========================================================================
FUNCTION MMGSDI_SFS_RMFILE

DESCRIPTION
  Generic Function called by GSDI to delete a file from SFS.  This function
  will validate the input parameters provided and return a status when
  deleted.

DEPENDENCIES
  The file does not exist.

RETURN VALUE
  mmgsdi_return_enum_type:  MMGSDI_SUCCES:    File was removed
                            MMGSDI_ERROR:     File removal error

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_sfs_rmfile(
  const char* sfs_file_name
);

/*===========================================================================
FUNCTION MMGSDI_SFS_WRITE

DESCRIPTION
  This function will write to the sfs file.


DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type:  MMGSDI_SUCCES:    Successful write
                            MMGSDI_ERROR:     SFS error
                            MMGSDI_NOT_FOUND: Unable to open file
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_sfs_write(
  uint8      *buffer_ptr,
  uint32     *len,
  const char *sfs_file_name
);

/*===========================================================================
FUNCTION MMGSDI_SFS_READ

DESCRIPTION
  This function will read from the sfs file.


DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type:  MMGSDI_SUCCES:    Successful read
                            MMGSDI_ERROR:     SFS error
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_sfs_read(
  uint8      *buffer_ptr,
  uint32      len,
  const char *sfs_file_name
);

#endif /* MMGSDI_SFS_H */

