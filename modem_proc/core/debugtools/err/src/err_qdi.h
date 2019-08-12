/*=============================================================================

                err_qdi.h  --  Header File

GENERAL DESCRIPTION
      ERR QDI Layer Header File

EXTERNAL FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

Copyright (c) 2014
by QUALCOMM Technologies Incorporated.  All Rights Reserved.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.


$Header: //components/rel/core.mpss/3.5.c12.3/debugtools/err/src/err_qdi.h#1 $ 
$DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
007/31/14   din     File created to support Error on User PD.
=============================================================================*/
#ifndef ERR_QDI_H
#define ERR_QDI_H

#include "qurt_qdi.h"
#include "qurt_qdi_driver.h"
#include "qurt.h"
#include "comdef.h"
#include "erri_sizes.h"
#include "DDITimetick.h"
#include "timer.h"


#define ERR_DRIVER_NAME                    "/dev/err_qdi_pd"

#define ERR_QDI_SEND_PDR_INFO              (0+QDI_PRIVATE)
#define ERR_QDI_INFORM_PFR                 (1+QDI_PRIVATE)
#define ERR_QDI_ERR_HANDLING_DONE          (2+QDI_PRIVATE)


#define ERR_QDI_DEVICE_NAME_LEN            (QURT_MAX_NAME_LEN*2)

typedef enum
{
  ERR_QDI_USERPD_NOT_INITIALIZED = 0,
  ERR_QDI_USERPD_INITIALIZED,
  ERR_QDI_USERPD_ERR_INITIATED,
  ERR_QDI_USERPD_ERR_HANDLING_COMPLETED,
  ERR_QDI_USERPD_ERR_PD_EXIT_STARTED,
  ERR_QDI_USERPD_ERR_PD_EXIT_COMPLETED
} err_qdi_userpd_state_e;

typedef struct err_qdi_internal_struct
{
  char                    pd_name[ERR_QDI_DEVICE_NAME_LEN];
  uint32                  pid;
  char                    pfr[ERR_QDI_PFR_LEN];
  uint32                  pfr_len;
  DalTimetickTime64Type   start_time;
  DalTimetickTime64Type   end_time;
  err_qdi_userpd_state_e  state;
  timer_type              pdtimeout_timer;
  boolean                 pdtimeout_timer_set;
  boolean                 pdr_enabled;
} err_qdi_internal_s;



typedef enum err_qdi_enum {
  ERR_QDI_SUCCESS = 0,
  ERR_QDI_FAIL,
  ERR_QDI_INVALID_PARAMETERS,
  ERR_QDI_NOT_INITIALIZED
}err_qdi_e;



#endif /* ERR_QDI_H */
