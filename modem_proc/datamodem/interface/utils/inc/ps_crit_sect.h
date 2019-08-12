#ifndef PS_CRIT_SECT_H
#define PS_CRIT_SECT_H
/*===========================================================================

                           P S _ C R I T _ S E C T . H

GENERAL DESCRIPTION
  Collection of Critical Section Macros used by various modules in PS.
  
Copyright (c) 2008 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_utils.h_v   1.0   08 Aug 2002 11:19:58   akhare  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/utils/inc/ps_crit_sect.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when      who     what, where, why
-------   ---     ----------------------------------------------------------
12/03/09  hm      Make critical sections as functions that can be 
                  implemented in platform specific way.
12/17/08  pp      Created as part of CMI: Public/Private API split.
===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

                            PUBLIC DATA DECLARATIONS

===========================================================================*/
/*!
  @brief 
  Definition of the PS critical section type.
  
  @details
  This definition is provided for allowing different platform specific
  critical section implementation. The handle variable inside the struct
  refers to the platform specific implementation of the critical section.
*/
typedef struct ps_crit_sect_s
{
  void* handle;
} ps_crit_sect_type;

/*!
  Declaration of global PS critical section.
*/
extern ps_crit_sect_type global_ps_crit_section;

/*!
  Declaration of global DS_UTILS critical section, this macro is used only
  for ref-count update serialization in DSNET/DSSOCK/UTILS layers. Do not
  use this for any other purpose.
*/
extern ps_crit_sect_type global_ds_utils_crit_section;

/*===========================================================================

                          PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/*!
  @brief 
  Initialize a critical section

  @params[in] crit_sect_ptr
  Critical section pointer to be initialized, client just need to pass a 
  structure of type ps_crit_sect_type, memory for any implementation 
  specific data is allocated by platforms. 

  @return
  NONE.

  @notes
  This function should have been called before any calls to
  PS_ENTER_CRIT_SECT() or PS_LEAVE_CRIT_SECT().
  After the critical section usage is complete, client should call
  PS_DESTROY_CRIT_SECTION to release the critical section and its 
  associated memory.
*/
#define PS_INIT_CRIT_SECTION ps_init_crit_section
void ps_init_crit_section
(
  ps_crit_sect_type*  crit_sect_ptr
);


/*!
  @brief 
  Enter a critical section

  @params[in] crit_sect_ptr
  Critical section to be entered. 

  @return
  NONE.

  @notes
  Critical section should be initialized prior to calling this function. 
*/
#define PS_ENTER_CRIT_SECTION ps_enter_crit_section
void ps_enter_crit_section
(
  ps_crit_sect_type*  crit_sect_ptr
);

/*!
  @brief 
  Leave a critical section

  @params[in] crit_sect_ptr
  Critical section to be left.

  @return
  NONE.

  @notes
  Critical section should be initialized prior to calling this function. 
  If not already in critical section, the behavior is undefined.
*/
#define PS_LEAVE_CRIT_SECTION ps_leave_crit_section
void ps_leave_crit_section
(
  ps_crit_sect_type*  crit_sect_ptr
);

/*!
  @brief 
  Destroy a critical section

  @params[in] crit_sect_ptr
  Critical section to be destroyed.

  @return
  NONE.

  @notes
  Should be only called on initialized critical sections. Otherwise
  behavior is undefined.
*/
#define PS_DESTROY_CRIT_SECTION ps_destroy_crit_section
void ps_destroy_crit_section
(
  ps_crit_sect_type*  crit_sect_ptr
);


/*---------------------------------------------------------------------------
  Legacy critical section variations. 
  These are being deprecated and should not be used.
---------------------------------------------------------------------------*/
#define PS_BRANCH_ENTER_CRIT_SECTION(cs_p)      PS_ENTER_CRIT_SECTION(cs_p)
#define PS_BRANCH_LEAVE_CRIT_SECTION(cs_p)      PS_LEAVE_CRIT_SECTION(cs_p)
#define PS_ENTER_CRIT_SECTION_NO_TASKLOCK(cs_p) PS_ENTER_CRIT_SECTION(cs_p)
#define PS_LEAVE_CRIT_SECTION_NO_TASKLOCK(cs_p) PS_LEAVE_CRIT_SECTION(cs_p)


#ifdef __cplusplus
}
#endif

#endif /* PS_CRIT_SECT_H */

