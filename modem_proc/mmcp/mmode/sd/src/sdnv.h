#ifndef SDNV_H
#define SDNV_H
/*===========================================================================

               S Y S T E M   D E T E R M I N A T I O N

          N V   I N T E R F A C E   H E A D E R   F I L E

DESCRIPTION
  This header file contains all the definitions necessary to interface
  with SDNV.C (i.e. the NV-Interface component of the SD).

Copyright (c) 2000 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/sd/src/sdnv.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/10/11   am      Migrating SUTE to QTF
08/12/08   fj      Modified sdnv_write() return value to boolean, to support
                   prl_commit2().
08/15/06   pk      UOOS for GW modifications, added support to read GCF flag
                   from NV
07/15/03   JQ      Added Extended PRL support
                   - D PRL support.
01/25/02   sj      Added support for PUZL - user zone feature.
02/26/01   RC      Changes to support dual-mode targets.
11/17/00   RC      Initial release.

===========================================================================*/


#include "mmcp_variation.h"
#include "sd.h"       /* External interface to SD */
#include "sdi.h"      /* Internal interface to sd.c */

#include "comdef.h"   /* Definition for basic types and macros */
#include "nv.h"       /* Interface to NV services */




/* <EJECT> */
/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/* Type for buffering SD related NV items while doing NV read/write
** operations.
**
** The only purpose of this type is to create a big enough buffer to hold
** data that is associated with SD NV items.
**
** Note that we could simply use nv_item_type, but this would be a waste
** of ~300 bytes of RAM.
**
** Note that Lint info 754 (local structure member not referenced) is
** is expected here.
*/
typedef struct {  /*lint -e754 */

  union {

    nv_mru3_table_type          mru_tbl;          /* MRU table */
    nv_home_sid_nid_type        home_sid_nid_lst; /* Home SID/NID list */
    nv_sid_type                 amps_home_sid;    /* AMPS home SID */
    nv_cdmach_type              cdmach;           /* Pri/Sec A/B channels */
    nv_roaming_list_union_type  roaming_list;     /* PRL header size */
    nv_sid_nid_lock_type        sid_nid_lock;     /* Lockout SID/NID list */
    boolean                     gprs_anite_gcf;   /* Enable Anite GCF 51.010 */

  } buffer;

  byte    padding[4];

} sdnv_data_s_type;




/* <EJECT> */
/*===========================================================================

FUNCTION sdnv_init

DESCRIPTION
  Initialize the SD NV-Interface component.

  This function must be called before the NV interface block
  is being used in any way, place or form.

DEPENDENCIES
  This function must be called before any other sdnv_xxx() function is
  ever called.

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern  void                      sdnv_init(

        rex_sigs_type             nv_sig,
            /* Rex signal to be used by SD for waiting on NV to completes a
            ** read/write operation.
            */

        rex_sigs_type             dog_sig,
            /* Rex signal to be used by SD for watchdog reporting while SD
            ** waits on NV to complete a read/write operation.
            */

        void                      (*dog_rpt_func_ptr) (void)
            /* Pointer to a watchdog reporting function to be called by SD
            ** when the watchdog sig is set while SD waits on NV to complete
            ** a read/write operation.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdnv_write

DESCRIPTION
  Write a SD related item to NV memory.

  Note that the NV write is done in a synchronous fashion. That is, this
  function only returns after the NV task completes the write operation.

DEPENDENCIES
  sdnv_init() must have already been called to initialize the SD NV-Interface
  component.

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern  boolean                      sdnv_write(

        nv_items_enum_type        nv_item,
            /* NV item to write.
            */

        nv_item_type              *data_ptr
            /* Pointer to the data that is associated with the NV item.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdnv_read

DESCRIPTION
  Read a SD related item to NV memory.

  Note that the NV read is done in a synchronous fashion. That is, this
  function only returns after the NV task completes the read operation.

DEPENDENCIES
  sdnv_init() must have already been called to initialize the SD NV-Interface
  component.

RETURN VALUE
  TRUE if NV read operation is successful (or alternatively the NV items is
  being set to a default value). FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                  sdnv_read(

        nv_items_enum_type        nv_item,
            /* NV item to read.
            */

        nv_item_type              *data_ptr
            /* Pointer to a data buffer where to store the data that is
            ** associated with the NV item.
            */
);

#endif /* SDNV_H */
