#ifndef QMI_CRIT_SECT_H
#define QMI_CRIT_SECT_H
/*===========================================================================

                           QMI _ C R I T _ S E C T . H

GENERAL DESCRIPTION
  Collection of Critical Section Macros used by QMI modules .
  
Copyright (c) 2011 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/utils/inc/qmi_crit_sect.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when      who     what, where, why
-------   ---     ----------------------------------------------------------
09/27/13  wc      Support dynamic port configuration
03/02/10  rt      Created Module. (Critical sections made as functions so
                  that they can be implemented in platform specific way)
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
  Definition of the QMI critical section type.
  
  @details
  This definition is provided for allowing different platform specific
  critical section implementation. The handle variable inside the struct
  refers to the platform specific implementation of the critical section.
*/
typedef struct qmi_crit_sect_s
{
  void* handle;
} qmi_crit_sect_type;

/*!
  Declaration of global QMI critical section.
*/
extern qmi_crit_sect_type global_qmi_crit_section;
extern qmi_crit_sect_type qmi_port_cfg_crit_section;

/*===========================================================================

                          PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/*!
  @brief 
  Initialize a critical section

  @params[in] crit_sect_ptr
  Critical section pointer to be initialized, client just need to pass a 
  structure of type qmi_crit_sect_type, memory for any implementation 
  specific data is allocated by platforms. 

  @return
  NONE.

  @notes
  This function should have been called before any calls to
  QMI_ENTER_CRIT_SECT() or QMI_LEAVE_CRIT_SECT().
  After the critical section usage is complete, client should call
  QMI_DESTROY_CRIT_SECTION to release the critical section and its 
  associated memory.
*/
#define QMI_INIT_CRIT_SECTION qmi_init_crit_section
void qmi_init_crit_section
(
  qmi_crit_sect_type*  crit_sect_ptr
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
#define QMI_ENTER_CRIT_SECTION qmi_enter_crit_section
void qmi_enter_crit_section
(
  qmi_crit_sect_type*  crit_sect_ptr
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
#define QMI_LEAVE_CRIT_SECTION qmi_leave_crit_section
void qmi_leave_crit_section
(
  qmi_crit_sect_type*  crit_sect_ptr
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
#define QMI_DESTROY_CRIT_SECTION qmi_destroy_crit_section
void qmi_destroy_crit_section
(
  qmi_crit_sect_type*  crit_sect_ptr
);


/*---------------------------------------------------------------------------
  Legacy critical section variations. 
  These are being deprecated and should not be used.
---------------------------------------------------------------------------*/
#define QMI_BRANCH_ENTER_CRIT_SECTION(cs_p)      QMI_ENTER_CRIT_SECTION(cs_p)
#define QMI_BRANCH_LEAVE_CRIT_SECTION(cs_p)      QMI_LEAVE_CRIT_SECTION(cs_p)
#define QMI_ENTER_CRIT_SECTION_NO_TASKLOCK(cs_p) QMI_ENTER_CRIT_SECTION(cs_p)
#define QMI_LEAVE_CRIT_SECTION_NO_TASKLOCK(cs_p) QMI_LEAVE_CRIT_SECTION(cs_p)


#ifdef __cplusplus
}
#endif

#endif /* QMI_CRIT_SECT_H */
