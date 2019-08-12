#ifndef COMPTASK_V_H
#define COMPTASK_V_H
/*===========================================================================

                              C O M P  T A S K

                            H E A D E R   F I L E

DESCRIPTION
  This is the external header file for the COMP Task. This file
  contains all the functions, definitions and data types needed for other
  tasks to interface to the COMP Task.

  Signal and command definitions for the COMP Task, and functions
  that should be used by other tasks to send commands to the COMP
  Task are defined here.

  Copyright (c) 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.  
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/ps/inc/comptask_v.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/22/12   vb	   
11/11/10   sch     Moving comp_sig_handler_type, comp_set_sig_handler, 
                   comp_reserve_to_comp_tx_sig to comptask_api.h
05/09/10   vrk     Moving COMP_SET/CLR_SIGNAL to comptask_api.h
10/20/09   ss      Moving declaration of comp_get_cmd_buf() to comptask_api.h.
06/23/09   ss      SU level CMI modifications.
03/04/09   sa      AU level CMI modifications.
11/06/06   vk      Added Data on Apps support.
09/01/06   rsl     Added rab_dl_rate, rab_ul_rate and rohc_target_mode to
                   cfg_req_type to support dynamic WM level tuning and
                   to be able to set ROHC decompressor mode.
08/31/06   rsl     Featurized PDCP
08/18/06   rsl     Added support for traffic class under 
                   FEATURE_CPU_BASED_FLOW_CONTROL.

===========================================================================*/


/*===========================================================================

                      INCLUDE FILES

===========================================================================*/
/*lint -e617*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#if defined (FEATURE_DATA_WCDMA_PS) || defined (FEATURE_GSM_GPRS)

#include "queue.h"
#include "task.h"
#include "dsm.h"
#include "dsm_queue.h"
#include "comptask.h"
#include "comptask_api.h"
#include "pdcpi.h"
#include "ps_rohc.h"

#ifdef FEATURE_CPU_BASED_FLOW_CONTROL
#include "dsumtspdpreg.h"
#endif

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
typedef struct
{
  comp_sig_handler_type sig_handler;
  void *user_data_ptr;
} comp_sig_handler_info_type;

extern comp_sig_handler_info_type comp_sig_handler[COMP_MAX_SIGNALS];

#endif /* FEATURE_DATA_WCDMA_PS || FEATURE_GSM_GPRS */

#endif /* COMPTASK_V_H */
