//source/qcom/qct/modem/utils/api/main/latest/fc.h#1 - branch change 1605186 (ktext)
#ifndef FC_H__
#define FC_H__
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              F L O W    C O N T R O L L E R     H E A D E R    F I L E

DESCRIPTION
  Public Header file for the Flow Contoller. Contains types and externalized
  functions of the Flow Controller module.

Copyright (c) 2000-2006 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/utils/api/fc/fc.h#1 $ $DateTime: 2016/12/13 07:57:51 $ $Author: mplcsds1 $

when       who      what, where, why
--------   -------- --------------------------------------------------------
12/02/11    grk      Removed featurization for FEATURE_CPU_BASED_FLOW_CONTROL
05/21/11    kvk      Moved fc_get_out_cmd_buf from static to extern
07/26/11    ssg      Added new api: fc_free_out_cmd to free memory.
09/06/10    pj       Fixed KW warnings
09/23/09    pj       Mainlined "FEATURE_FC_CPU_MONITOR" 
06/15/09    pj       Added proper Featurization for code under "FEATURE_FC_CPU_MONITOR"
05/29/09    pj       Added support for pre-allocated command buffers
05/05/09    pj       Added support for event-based/ on-demand reporting over RPC-interface
04/13/09    pj       Added MACRO to indicate processor-id 
01/02/09    pj       Added support for RPC calls
11/14/08    pj       Added support for CPU_MONITOR requirements
04/25/08    ar       Introduced fc_cpu_threshold_value_type for storing the
                     cpu threshold values of registered clients
01/31/07    gsc      Added fc_enable() and fc_disable()
08/09/06    gsc      Initial Revision

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"

#ifdef CUST_H
#include "customer.h"
#endif

#include "rex.h"
#include "queue.h"


#include "cpu_monitor_modem.h"


#define FC_OUT_CMD_MAX_BUF            20
extern q_type                         fc_out_cmd_free_q;
extern fc_out_cmd_type                fc_out_cmd_buf[FC_OUT_CMD_MAX_BUF];
extern uint32                         fc_dbg_var;

/*===========================================================================
FUNCTION:        fc_get_in_cmd_buf

DESCRIPTION:
  This function is used by the clients to allocate a memory buffer to send
  commands to the Flow Controller. The memory is freed when the command is
  processed by the Flow Controller

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===========================================================================*/
extern fc_in_cmd_type *fc_get_in_cmd_buf( void );


/*==============================================================================================

FUNCTION:  fc_get_out_cmd_buf

DESCRIPTION:
  This function gets the command buffer for a command to be sent from the Flow Controller.

DEPENDENCIES:
  None.

RETURN VALUE:
  Pointer to the allocated buffer if successful, otherwise NULL

SIDE EFFECTS
  None.
=============================================================================================*/
extern fc_out_cmd_type *fc_get_out_cmd_buf( void );


/*===========================================================================
FUNCTION: fc_put_in_cmd

DESCRIPTION:
  This function is used by the clients of Flow Controller to send a command
  to the Flow Controller

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None

===========================================================================*/
extern void fc_put_in_cmd( fc_in_cmd_type *cmdptr );


/*===========================================================================
FUNCTION: fc_enable

DESCRIPTION:
  This function enables Flow Controller

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===========================================================================*/
extern void fc_enable (void);

/*===========================================================================
FUNCTION: fc_disable

DESCRIPTION:
  This function disables Flow Controller

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===========================================================================*/
extern void fc_disable (void);

/*=============================================================================

FUNCTION:  fc_put_cmd_buf_in_free_q

DESCRIPTION:
  This function puts the command buffer to free-q 

DEPENDENCIES:
  None.

RETURN VALUE:
  None

SIDE EFFECTS
  None.
==============================================================================*/
extern void fc_put_cmd_buf_in_free_q (fc_out_cmd_type *cmd_ptr);

/*===========================================================================
FUNCTION: fc_update_remote_cpu_monitor

DESCRIPTION:
  This function is used to export CPU loading to other processors 
  through RPC mechanisms

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===========================================================================*/
boolean fc_update_remote_cpm_clnt (
          fc_register_type     reg_data, 
          fc_out_cmd_enum_type out_cmd,
          uint8                cpu_load_val);

/*===========================================================================
FUNCTION: fc_debug_stubs

DESCRIPTION:
  This function is a stub, to validate CPU MONITOR functionality.

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===========================================================================*/
void fc_debug_stub(fc_processor_type proc_idx);

/*==============================================================================================

FUNCTION:  fc_free_out_cmd

DESCRIPTION:
  This function frees the memory allocated for the command

DEPENDENCIES:
  None.

RETURN VALUE:
  None

SIDE EFFECTS
  None.
=============================================================================================*/
extern void fc_free_out_cmd(fc_out_cmd_type *cmd_ptr);

#endif /* FC_H__*/
