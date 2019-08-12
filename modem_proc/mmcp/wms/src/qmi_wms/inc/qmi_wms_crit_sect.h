#ifndef QMI_WMS_CRIT_SECT_H
#define QMI_WMS_CRIT_SECT_H
/*===========================================================================

                           Q M I _ W M S _ C R I T _ S E C T . H

GENERAL DESCRIPTION
  Collection of Critical Section Macros used by QMI WMS SVC modules .

Copyright (c) 2011 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/wms/src/qmi_wms/inc/qmi_wms_crit_sect.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when      who     what, where, why
-------   ---     ----------------------------------------------------------
03/02/10  rt      Created Module. (Critical sections made as functions so
                  that they can be implemented in platform specific way)
===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"


/*===========================================================================

                            PUBLIC DATA DECLARATIONS

===========================================================================*/
/*!
  @brief
  Definition of the QMI WMS critical section type.

  @details
  This definition is provided for allowing different platform specific
  critical section implementation. The handle variable inside the struct
  refers to the platform specific implementation of the critical section.
*/
typedef struct qmi_wms_crit_sect_s
{
  void* handle;
} qmi_wms_crit_sect_type;

/*!
  Declaration of global QMI critical section.
*/
extern qmi_wms_crit_sect_type global_qmi_wms_crit_section;

/*===========================================================================

                          PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/*!
  @brief
  Initialize a critical section

  @params[in] crit_sect_ptr
  Critical section pointer to be initialized, client just need to pass a
  structure of type qmi_wms_crit_sect_type, memory for any implementation
  specific data is allocated by platforms.

  @return
  NONE.

  @notes
  This function should have been called before any calls to
  QMI_WMS_ENTER_CRIT_SECT() or QMI_WMS_LEAVE_CRIT_SECT().
  After the critical section usage is complete, client should call
  QMI_WMS_DESTROY_CRIT_SECTION to release the critical section and its
  associated memory.
*/
void qmi_wms_init_crit_section
(
  qmi_wms_crit_sect_type*  crit_sect_ptr
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
void qmi_wms_enter_crit_section
(
  qmi_wms_crit_sect_type*  crit_sect_ptr
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
void qmi_wms_leave_crit_section
(
  qmi_wms_crit_sect_type*  crit_sect_ptr
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
void qmi_wms_destroy_crit_section
(
  qmi_wms_crit_sect_type*  crit_sect_ptr
);
#endif /* QMI_WMS_CRIT_SECT_H */

