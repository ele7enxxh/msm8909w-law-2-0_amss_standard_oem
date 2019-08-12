#ifndef NVRUIM_H
#define NVRUIM_H
/*==========================================================================

         N V    R - U I M    I N T E R N A L    H E A D E R    F I L E

DESCRIPTION
  This header file contains all the internal interface definitions for
  the NVRUIM subsystem that are used within NVRUIM.

REFERENCES
  IS-820, Removable User Identity Module (RUIM) for Spread Spectrum Systems

Copyright (c) 2009 - 2016 by QUALCOMM Technologies Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                        EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/nvruim/src/nvruim.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/18/16   nr      Redirect requests to NV during QCN Restore operation
07/15/14   yt      Support for NV refresh
04/01/14   av      Moving definitions used by NVRUIM task under nvruim/src
02/22/14   av      Replace UIM enums with MMGSDI enums
10/17/13   at      SIM busy support in NVRUIM
05/13/13   vs      Decoupling NV-EFS from NV-RUIM
05/10/12   tl      Fixed MCC 414 to correct spec value
04/25/12   tl      Added support for MCC 0x158 and 0x19E
03/12/12   yt      Move data types from public header
11/11/11   ssr     Ignore the fallback request for non CT card
06/27/11   yt      Removed featurization from UIM public headers
05/16/11   ssr     Added NVRUIM thread safe support
02/08/11   ssr     Moved HRPD check from nvruim to mmgsdi layer
11/02/10   rm      Externize the nv_rtre_control API
08/12/10   ssr     Fixed nvruim 1x primary session for slot2
08/10/10   js      Updated nvruim get data sync to support 3gpd control data
06/18/10   ssr     Add non 3gpd cdma card check
06/17/10   js      Support for fetching nvruim data
                   on fusion
05/13/10   vs      Fix for NV and GSDI deadlock
04/30/10   ssr     Fixed nvruim clear cache
04/23/10   ssr     Fixed UIM 3GPD NV support
02/18/10   vs      Subscription mask fix
02/15/10   nb      Moved for Dual Slot Changes
11/17/09   yb      Moving nvruim_init_operator_card to nvruimi_v.h
09/30/09   mib     Added PACKED_POST for Q6 compilation
08/24/09   ssr     Fixed compilation error
02/24/09   nb      Initial Revision

===========================================================================*/


/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "comdef.h"
#include "nv.h"
#include "mmgsdilib.h"

/*===========================================================================

                           DATA DECLARATIONS

===========================================================================*/
/* ----------------------------------------------------------------------------

   ENUM:      NV_RUIM_SUPPORT_STATUS

   DESCRIPTION:
     Status as to whether or not a particular NV item is supported by the
     R-UIM subsystem.
-------------------------------------------------------------------------------*/
typedef enum {
  NV_RUIM_ITEM_NOT_SUPPORTED  = 0,    /* item is not supported by R-UIM  */
  NV_RUIM_SUPPORTS_ITEM       = 1     /* item is supported by R-UIM      */
} nv_ruim_support_status;

/* ----------------------------------------------------------------------------

   ENUM:      NV_RUIM_QCN_RESTORE_STATUS

   DESCRIPTION:
      Status of qcn restore in progress, as read from EFS 
      /nvruim/qcn_restore_inprogress location.
-------------------------------------------------------------------------------*/
typedef enum {
  NV_RUIM_QCN_RESTORE_UNKNOWN         = 0,  /* Restore status unknown  */
  NV_RUIM_QCN_RESTORE_IN_PROGRESS     = 1,  /* Restore in progress        */
  NV_RUIM_QCN_RESTORE_NOT_IN_PROGRESS = 2   /* Restore not in progress   */
} nv_ruim_qcn_restore_status;

/* EFS file which indicates QCN Restore in progress */
#define NVRUIM_QCN_RESTORE_IN_PROGRESS_EFS "/nvruim/qcn_restore_inprogress"
#define NVRUIM_EFS_DIR                     "/nvruim"

/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================

FUNCTION NVRUIM_WRITE

DESCRIPTION
  Check to see if the item is supported by the R-UIM.
  If it is supported, write it.

DEPENDENCIES
  None

RETURN VALUE
  NV_RUIM_ITEM_NOT_SUPPORTED    Item is not supported
  NV_RUIM_SUPPORTS_ITEM         Item is supported, see op_status

  op_status indicates the success/failure of the write.  For items
  which are "not supported", the value of op_status is undefined.

SIDE EFFECTS
  None

===========================================================================*/
nv_ruim_support_status nvruim_write
(
  nv_cmd_type  *nv_cmd_ptr,       /* command block               */
  nv_stat_enum_type *op_status    /* status of the I/O operation */
);

/*===========================================================================

FUNCTION NVRUIM_READ

DESCRIPTION
  Check to see if the item is supported by the R-UIM.
  If it is supported, read it.

DEPENDENCIES
  None

RETURN VALUE
  NV_RUIM_ITEM_NOT_SUPPORTED    Item is not supported
  NV_RUIM_SUPPORTS_ITEM         Item is supported, see op_status

  op_status indicates the success/failure of the read.  For items
  which are "not supported", the value of op_status is undefined.

SIDE EFFECTS
  None

===========================================================================*/
nv_ruim_support_status nvruim_read
(
  nv_cmd_type  *nv_cmd_ptr,       /* command block               */
  nv_stat_enum_type *op_status    /* status of the I/O operation */
);

/*===========================================================================

FUNCTION nvruim_is_bcsms_svc_avail

DESCRIPTION
  Gets the nvruim_bcsms_svc state

RETURN VALUE
  Boolean

DEPENDENCIES/SIDE EFFECTS/NOTES
  None

===========================================================================*/
boolean nvruim_is_bcsms_svc_avail
(
  void
);

/*===========================================================================

FUNCTION nvruim_is_smscap_svc_avail

DESCRIPTION
  Gets the nvruim_smscap_svc state

RETURN VALUE
  Boolean

DEPENDENCIES/SIDE EFFECTS/NOTES
  None

===========================================================================*/
boolean nvruim_is_smscap_svc_avail
(
  void
);

/*===========================================================================

FUNCTION NVRUIM_IS_LBS_SUPPORT_AVAIL

DESCRIPTION
  The function returns whether or not the RUIM/CSIM card have support for the
  LBS service the service table

RETURN VALUE
  Boolean

DEPENDENCIES/SIDE EFFECTS/NOTES
  None

===========================================================================*/
boolean nvruim_is_lbs_support_avail
(
  void
);

/*===========================================================================

FUNCTION NVRUIM_SEND_MMGSDI_CANCEL

DESCRIPTION
  This function is responsible for cancelling NVRUIM's pending command(s)
  with MMGSDI.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void nvruim_send_mmgsdi_cancel
(
  void
);

/*===========================================================================

FUNCTION nvruim_is_mmgsdi_item_enabled

DESCRIPTION
  Checks if the NV/EFS item is enabled for the appropriate index

RETURN VALUE
  Boolean

DEPENDENCIES/SIDE EFFECTS/NOTES
  None

===========================================================================*/
boolean nvruim_is_mmgsdi_item_enabled
(
  mmgsdi_cached_nv_item_enum_type  item
);


/*===========================================================================

FUNCTION NVRUIM_MEM_ALLOC

DESCRIPTION
  This function calls modem_mem_calloc() to allocate from Modem Heap.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  a pointer is allocated.
===========================================================================*/
void * nvruim_mem_malloc
(
  dword size
);

/*===========================================================================

FUNCTION NVRUIM_MEM_FREE_NULL_OK

DESCRIPTION
  The NVRUIM_MEM_FREE free the pointer from the Modem Heap. If the pointer
  intended to be free'd is already NULL, the functions just prints a
  message and returns.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  the pointer is freed, if non-null.
===========================================================================*/
void nvruim_mem_free_null_ok
(
  void * ptr
);

#endif /* NVRUIMI_H */
