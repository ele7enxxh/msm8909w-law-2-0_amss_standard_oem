#ifndef _SMD_LOOPBACK_SERVER_APIS_H_
#define _SMD_LOOPBACK_SERVER_APIS_H_

/*===========================================================================

      S M D   L O O P B A C K   S E R V E R   I N T E R N A L   A P I S

===========================================================================*/
/**
  @file smd_loopback_server_apis.h
*/
/* 
  Declares the private API used to implement the N-way SMD loopback server 
  actions, for each supported API.
*/

/*===========================================================================
    Copyright (c) 2011-2012 Qualcomm Technologies Incorporated.
    All Rights Reserved.
    Qualcomm Confidential and Proprietary.
===========================================================================*/


/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/rpm.bf/2.1.1/core/mproc/smd/src/smd_loopback_server_apis.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/31/12   pa      Updated smd_lb_server_lite_open API
07/01/11   bt      Initial implementation.
===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
/* All dependent headers should be included in the source files that include 
 * this header, above it. */

/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================
  FUNCTION  smd_lb_gen_close_cb
===========================================================================*/
/**
  This is the general server-side close callback operation, and is 
  agnostic to the SMD API used for the port.
  The only thing done is to set the server's state to 
  SMD_LOOPBACK_STATE_CLOSED, indicate that the close transition is done, and 
  notify the client.  Some APIs may require further cleanup after this 
  function is called.
  
  @param[in] idx  The index into the loopback command type register.

  @return         The smd_loopback_command_type structure at index idx.
  
  @sideeffects
  Interrupts the client processor.
*/
/*==========================================================================*/
void smd_lb_gen_close_cb ( uint32 idx );

/*===========================================================================
  FUNCTION  smd_lb_server_memcpy_open
===========================================================================*/
/**
  Opens an SMD loopback port, associated with the loopback command structure
  at the indicated index, using the memcpy API.  The fields in
  the loopback command structure determine the port properties.

  Sets the appropriate server callbacks.  Also sets the signal fields in the 
  loopback command structure based on the expected signals for memcpy, which
  is all starting at FALSE.

  @param[in] lb_cmd_info   The loopback command structure for this port.
  @param[in] lb_port_info  The internal server data structures for this port.
  @param[in] idx           The index at which to find the loopback command 
                           structure for the port to be opened (used in 
                           callbacks).

  @return    TRUE if the open was successful.
             FALSE if the open fails for any reason, based on the arguments 
             provided in the loopback command structure.

  @sideeffects
  Allocates an internal data structure from heap memory.
*/
/*=========================================================================*/
boolean smd_lb_server_memcpy_open
(
  smd_loopback_command_type *lb_cmd_info,
  smd_loopback_info_type    *lb_port_info,
  uint32                     idx
);

/*===========================================================================
  FUNCTION  smd_lb_server_lite_open
===========================================================================*/
/**
  Opens an SMD loopback port, associated with the loopback command structure
  at the indicated index, using the SMDL API.  The fields in
  the loopback command structure determine the port properties.

  Sets the appropriate server callbacks.  Also sets the signal fields in the 
  loopback command structure based on the expected signals for SMDL, which
  is DTR, CTS, and CD equal to TRUE and RI equal to FALSE.

  @param[in] lb_cmd_info   The loopback command structure for this port.
  @param[in] lb_port_info  The internal server data structures for this port.
  @param[in] idx           The index at which to find the loopback command 
                           structure for the port to be opened (used in 
                           callbacks).

  @return    NULL if the open fails for any reason, based on the arguments
             provided in the loopback command structure.
             Non-NULL if the open is successful.

  @sideeffects
  Allocates an internal data structure from heap memory.
*/
/*=========================================================================*/
smdl_loop_type * smd_lb_server_lite_open
(
  smd_loopback_command_type *lb_cmd_info,
  smd_loopback_info_type    *lb_port_info,
  uint32                     idx
);

/*===========================================================================
  FUNCTION  smd_lb_server_dsm_open
===========================================================================*/
/**
  Opens an SMD loopback port, associated with the loopback command structure
  at the indicated index, using the SMD DSM API.  The fields in
  the loopback command structure determine the port properties.

  Sets the appropriate server callbacks.  Also sets the signal fields in the 
  loopback command structure based on the expected signals for SMD DSM, which
  is all starting at FALSE.

  @param[in] lb_cmd_info   The loopback command structure for this port.
  @param[in] lb_port_info  The internal server data structures for this port.
  @param[in] idx           The index at which to find the loopback command 
                           structure for the port to be opened (used in 
                           callbacks).

  @return    TRUE if the open was successful.
             FALSE if the open fails for any reason, based on the arguments 
             provided in the loopback command structure.

  @sideeffects
  Allocates an internal data structure from heap memory.
*/
/*=========================================================================*/
boolean smd_lb_server_dsm_open
(
  smd_loopback_command_type *lb_cmd_info,
  smd_loopback_info_type    *lb_port_info,
  uint32                     idx
);

/*===========================================================================
  FUNCTION  smd_lb_server_memcpy_close
===========================================================================*/
/**
  Closes an SMD loopback port, associated with the indicated internal data 
  structure.

  When the server has finished the close operation, the local close callback 
  will run, which sets the server-side loopback state to 
  SMD_LOOPBACK_STATE_CLOSED, notifies the client, and does any needed cleanup.

  @param[in] lb_port_info  The internal server data structure for this port.

  @return    None.
  
  @sideeffects
  None.
*/
/*=========================================================================*/
void smd_lb_server_memcpy_close
(
  smd_loopback_info_type *lb_port_info
);

/*===========================================================================
  FUNCTION  smd_lb_server_lite_close
===========================================================================*/
/**
  Closes an SMD loopback port, associated with the indicated internal data 
  structure.

  When the server has finished the close operation, the local close callback 
  will run, which sets the server-side loopback state to 
  SMD_LOOPBACK_STATE_CLOSED, notifies the client, and does any needed cleanup.

  @param[in] lb_port_info  The internal server data structure for this port.

  @return    None.
  
  @sideeffects
  None.
*/
/*=========================================================================*/
void smd_lb_server_lite_close
(
  smd_loopback_info_type *lb_port_info
);

/*===========================================================================
  FUNCTION  smd_lb_server_dsm_close
===========================================================================*/
/**
  Closes an SMD loopback port, associated with the indicated internal data 
  structure.

  When the server has finished the close operation, the local close callback 
  will run, which sets the server-side loopback state to 
  SMD_LOOPBACK_STATE_CLOSED, notifies the client, and does any needed cleanup.

  @param[in] lb_port_info  The internal server data structure for this port.

  @return    None.
  
  @sideeffects
  None.
*/
/*=========================================================================*/
void smd_lb_server_dsm_close
(
  smd_loopback_info_type *lb_port_info
);

/*===========================================================================
  FUNCTION  smd_lb_server_memcpy_signal
===========================================================================*/
/**
  Sets all RS-232 signals of an SMD loopback port, associated with the 
  indicated loopback command structure and internal server structure.  The 
  values to set the signals to are obtained from this loopback command 
  structure, which may not necessarily be different from the port's current 
  values.

  @param[in] lb_cmd_info   The loopback command structure for this port.
  @param[in] lb_port_info  The internal server data structures for this port.

  @return    None.
  
  @sideeffects
  None.
*/
/*=========================================================================*/
void smd_lb_server_memcpy_signal
(
  smd_loopback_command_type *lb_cmd_info,
  smd_loopback_info_type    *lb_port_info
);

/*===========================================================================
  FUNCTION  smd_lb_server_lite_signal
===========================================================================*/
/**
  Sets all RS-232 signals of an SMD loopback port, associated with the 
  indicated loopback command structure and internal server structure.  The 
  values to set the signals to are obtained from this loopback command 
  structure, which may not necessarily be different from the port's current 
  values.

  @param[in] lb_cmd_info   The loopback command structure for this port.
  @param[in] lb_port_info  The internal server data structures for this port.

  @return    None.
  
  @sideeffects
  None.
*/
/*=========================================================================*/
void smd_lb_server_lite_signal
(
  smd_loopback_command_type *lb_cmd_info,
  smd_loopback_info_type    *lb_port_info
);

/*===========================================================================
  FUNCTION  smd_lb_server_dsm_signal
===========================================================================*/
/**
  Sets all RS-232 signals of an SMD loopback port, associated with the 
  indicated loopback command structure and internal server structure.  The 
  values to set the signals to are obtained from this loopback command 
  structure, which may not necessarily be different from the port's current 
  values.

  @param[in] lb_cmd_info   The loopback command structure for this port.
  @param[in] lb_port_info  The internal server data structures for this port.

  @return    None.
  
  @sideeffects
  None.
*/
/*=========================================================================*/
void smd_lb_server_dsm_signal
(
  smd_loopback_command_type *lb_cmd_info,
  smd_loopback_info_type    *lb_port_info
);

#ifdef __cplusplus
}
#endif

#endif /* _SMD_LOOPBACK_SERVER_APIS_H_ */
