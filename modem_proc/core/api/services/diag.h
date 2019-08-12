#ifndef DIAG_H
#define DIAG_H
/*==========================================================================

                     Diagnostic Task Header File

Description
  Global Data declarations of the diag_task.

Copyright (c) 1991-2013 by QUALCOMM Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

===========================================================================*/


/*===========================================================================

                         Edit History

      $Header: //components/rel/core.mpss/3.5.c12.3/api/services/diag.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
06/28/13   rh      Updated to QShrink 3.0
06/18/13   ph      Added new F3 mask check API.
05/09/13   sr      Added support for diag mask change notification for 
                   registered users   
04/15/13   sr      Support for WCDMA to register with Diag to obtain the 
                   frame num  
10/12/12   ph      Removed the references of peek and poke code.
10/03/12   rh      Added diag_svc_enabled to check whether Diag has been activated 
07/24/12   sa      Support for calling listeners with MSG_SPRINTF_* F3's
06/26/12   sa      Added new extended event/log APIs to remove listener
                   with listener callback and param as arguments.
06/13/12   sa      Support for Extended F3 listeners
05/16/12   rh      Support for F3 listeners
03/26/12   hvm     Extended Event/Log Listener APIs added
03/04/11   is      Add RIVA proc ID
09/14/10   is      New control channel and central routing features 
06/20/10   sg      Added prototype to diag_early_init()
09/30/09   as      Removed inclusion of customer.h  
10/01/09   JV      Moved prototypes for diag_change_default_sio_bitrate and
                   diag_record_dm_baudrate to diagcomm.h
09/29/09   vs      Added DAIG_NO_PROC for initializing master table
07/31/09   JV      Merged Q6 diag code back to mainline
07/31/09   JV      Removed warnings by making signatues of functions uniform
07/15/09   mad     Featured out inclusion of sio.h. Featured out 
                   diag_change_default_sio_bitrate and diag_record_dm_baudrate
07/14/09   mad     Featurized inclusion of customer.h
07/10/09   as      Mainlined code under FEATURE_HTORPC_METACOMMENTS and 
                   removed support for FEATURE_DIAG_SPC_TTL.
06/11/09   JV      Featurized inclusion of customer.h to ease merging of the 
                   Q6 branch
05/07/09   vk      removed dependencies from diag_v.h 
12/12/08   vg      split public and private information diag.h and diag_v.h
10/03/08   vg      Update configuration to enable msm 8200, 2 modem processors
12/22/06   as      Moved proc ID macros to diag.h
12/05/06   as      Added signal for Diag drain timer
11/21/06   as      Moved DIAG internal features from diagi.h to diag.h
03/30/05   sl      Added support for SPC_UNLOCK_TTL to unlock/lock the sp_state
10/17/03   ph      For Rel A builds, redirect to new MC subsystem command.
09/23/03    gr     Function prototypes related to changes that enable more
                   efficient detection of the condition where diag is out of
                   heap space.
08/20/01   jal     Changes to support more Diag packets.  Support for powerup
                   online/offline state, service programming lock state.
04/06/01   lad     Cosmetic changes.
02/23/01   lad     Rearchitected the diagnostics subsystem to be a service 
                   rather than a module.  All coupling to targets has been
                   moved to target-specific implementations.  This file now
                   contains an API.  No other information or coupling remains
                   except for featurized legacy code that impacts existing 
                   targets.
                   Old history comments have been removed since most of them
                   are no longer applicable.

===========================================================================*/

#include "comdef.h"
#include "qw.h"
#include "msg_diag_service.h" /* For msg_ext_store_type, msg_qsr_store_type */


#define DIAG_MODEM_PROC  0
#define DIAG_APP_PROC    1
#define DIAG_COMMON_PROC 2
#define DIAG_DUAL_PROC   2
#define DIAG_QDSP6_PROC  3
#define DIAG_RIVA_PROC   4
#define DIAG_NO_PROC     0xFF /* Use to initialize proc_id if user doesn't specify */

#define DIAGCOMM_CTRL_NO_PORT -1 /* Used to initialize diagpkt_master_table_type.port */

/* ---------------------------------------------------------------------------
** Security states.  Some packets are restricted based on security state.
** -------------------------------------------------------------------------*/
#define DIAG_SEC_UNLOCKED 0x00  /* Packet accepted if security is unlocked */
#define DIAG_SEC_LOCKED   0x01  /* Packet accepted if security is locked   */
#define DIAG_SEC_ANY      0xFF  /* Any security */

#define DIAG_SPC_UNLOCKED 0x00  /* SPC has been entered, valid service   */
                   /* programming code has been entered.    */
#define DIAG_SPC_LOCKED   0x01  /* SP is locked, no valid service        */
                   /* programming code has been entered yet */

#ifdef __cplusplus
extern "C"
{
#endif

  /*The following names are used while calling RC event APIs 
    to notify clients about mask change*/
#define DIAG_EVENT_MASK_CHANGE_NOTIFY "diag:event_mask_change"
#define DIAG_LOG_MASK_CHANGE_NOTIFY "diag:log_mask_change"
#define DIAG_MSG_MASK_CHANGE_NOTIFY "diag:msg_mask_change"

/* This structure is sent to an event listener when an event is processed.
   The implementation relies on the format of this structure.  It is 
   dangerous to change this format of this structure. */
typedef struct
{
  unsigned int event_id;    /* event ID */

  qword ts;         /* 8-byte CDMA time stamp. */

  uint8 length;     /* length of the payload */
  /* Payload of size 'length': */
  uint8 payload[255];       /* payload, if length > 0 */

}
diag_event_type;


typedef union
{
  /*cmd_code = DIAG_EXT_MSG_F */
  msg_ext_type       * ext_sprintf_msg;
  /* cmd_code = DIAG_EXT_MSG_TERSE_F */
  msg_ext_store_type * ext_msg;
  /* cmd_code = DIAG_QSR_EXT_MSG_TERSE_F
     You will need to check qsr_flag in structure to determine what it is */
  msg_qsr_store_type * qsr_msg; /* qsr_flag = QSR_BITFLAG_IS_HASH */
  /* cmd_code = DIAG_QSR_EXT_MSG_TERSE_F
     You will need to check qsr_flag in structure to determine what it is */
  msg_v3_store_type * qsr_v3_msg; /* qsr_flag = QSR_BITFLAG_IS_V3 */
}
diag_msg_store_type;

/* This structure is sent to an F3 listener when an F3 msg is processed. */
typedef struct
{
  unsigned int cmd_code; /* Specifies the type of F3 msg and union type 
                            to use from diag_msg_store_type. */
  diag_msg_store_type msg_store;
}
diag_msg_type;

typedef struct
{
  unsigned int ssid;
  unsigned int ss_mask;
}
f3_listener_ext_key_type;

/*Enum values used to specify version ID in extended log, event and msg APIs*/
typedef enum
{
  DIAG_EXT_API_VERSION_0 = 0, /*backwards compatibility case - 
                              void* pointer passed in the extended APIs
                               is considered as NULL*/
  DIAG_EXT_API_VERSION_1 = 1 /*The ID field will be supported in the header - 
                              void* pointer passed in the extended APIs
                              is considered as UINT32* where the uint32 value is ID*/   
} diag_ext_api_version_type;

typedef uint32(*diag_wcdma_wl1_frame_cb_type)(void);
/* ---------------------------------------------------------------------------
** Service programming states.  Some packets are restricted until a valid
**  Service Programming Code is entered.
** -------------------------------------------------------------------------*/
typedef uint8 diag_sp_type;
/*~ FIELD diag_event_type.payload VARRAY 255 LENGTH diag_event_type.length */


typedef uint8 diag_security_type;


/*===========================================================================

FUNCTION DIAG_GET_SECURITY_STATE

DESCRIPTION
  This function returns the current security state of the diagnostics service.
  Some packets may be restricted by security state.  The security state is
  unlocked by sending the Security Password.


===========================================================================*/
  diag_security_type diag_get_security_state (void);


/* ==================================================================
FUNCTION DIAG_ALLOC_CHECK
DESCRIPTION
   Checks if memory is available in the diag heap. The caller can
   specify an array of allocations to be made, and the function
   returns 0 if one or more of the requested allocations cannot be
   satisfied.
   NOTE: This function uses cached information about the heap to do
   the allocation check. If the cache is dirty, the function returns
   1 without updating the cache. So the user must not assume that all
   allocations will succeed even if this function returns 1.
===================================================================== */
  int diag_alloc_check (unsigned int num_allocs, uint32 *alloc_sizes);


/*===========================================================================

FUNCTION DIAG_GET_SP_STATE

DESCRIPTION
  This function returns the current service programming state of the
  diagnostics service.  Some packets may be restricted by SP state.
  The SP state is unlocked by sending the Service Programming Code.


===========================================================================*/
  diag_sp_type diag_get_sp_state (void);

  
/*===========================================================================

FUNCTION DIAG_CHECK_PASSWORD

DESCRIPTION
  This function checks the given password code, and returns TRUE if it matches.
  
  If 'code_ptr' is NULL, TRUE is returned is no code has yet been written.

===========================================================================*/
boolean diag_check_password (void *code_ptr);


/*===========================================================================

FUNCTION DIAG_TIMEOUT

DESCRIPTION
  This procedure makes the diag task time out in the event of a detected 
  security breach.  This security measure is intended to prevent automated
  password and SPC hacking.
  
  The diag task flush all outbound data, timeout for 10 seconds, then flush
  the RX queue.
 
===========================================================================*/
  void diag_timeout (void);



/* ==================================================================
FUNCTION DIAG_FREE
DESCRIPTION
   Frees a chunk of memory allocated using rex_malloc.
===================================================================== */
  void diag_free (void *mem_ptr);

/*===========================================================================

FUNCTION DIAG_PAUSE

DESCRIPTION
  This procedure suspends the caller's context of the DIAG task for 
  'duration' milliseconds.  
  
  This routine, unlike diag_suspend(), allows the DIAG task to continue
  funcitoning.  DIAG will continue to transmit data while pausing.
  
  If 'enable_rx_processing' is TRUE, the DIAG task will process and dispatch
  inbound packets as well as servicing TX data.  If FALSE, TX only is 
  serviced.
     
===========================================================================*/
  void diag_pause (unsigned int duration, boolean enable_rx_processing);

/*===========================================================================

FUNCTION DIAG_MASKS_ENABLED

DESCRIPTION
  This function is used to determine if Diag has been activated to process
  logs, events, and/or F3 messages.  

RETURN VALUE
  Boolean indicating the whether Diag has any logging services enabled.
  FALSE indicates that none are enabled.
  TRUE indicates that at least one feature is enabled.

===========================================================================*/								
  boolean diag_masks_enabled (void);

  /*=========================================================================== 
FUNCTION  DIAG_F3_TRACE_ENABLED_CHECK
 
DESCRIPTION
    This routine checks if the F3 trace functionality is enabled, based on its
    current mask settings.
    
RETURN VALUE
    TRUE,  F3 trace is enabled. 
    FALSE, F3 trace is not enabled.
===========================================================================*/
boolean diag_f3_trace_enabled_check(void);


/*===========================================================================

FUNCTION TYPE DIAG_IDLE_PROCESSING_FTYPE

DESCRIPTION
  A function type that, if registered, is called when the DIAG task has 
  idle processing time.

===========================================================================*/
  typedef void (*diag_idle_processing_ftype) (void *param);

/*===========================================================================

FUNCTION DIAG_IDLE_PROCESSING_REGISTER

DESCRIPTION
  This procedure registers a function pointer to be called when the DIAG 
  task has idle processing time.
  
  If multiple entries of the same entry are registered, the funciton will
  be called as many times as it is registered.
  
RETURN VALUE
  A boolean indicating success of the registration.  If no resources 
  are available, FALSE is returned.

===========================================================================*/
  boolean diag_idle_processing_register (diag_idle_processing_ftype fp,
                     void *param);

/*===========================================================================

FUNCTION DIAG_IDLE_PROCESSING_UNREGISTER

DESCRIPTION
  This procedure unregisters the specified function pointer.  The function
  will no longer be called during idle DIAG task time.
  
  This routine matches both 'fp' and 'param'.  'param' is not dereferenced.
  
  If multiple entries exist, this routine will only unregister the first
  entry found.

RETURN VALUE
  A boolean indicating success of the registration.  FALSE indicates the
  specified funciton/param pair are not registered.

===========================================================================*/
  boolean diag_idle_processing_unregister (diag_idle_processing_ftype fp,
                       void *param);

/*===========================================================================

FUNCTION DIAG_TX_NOTIFY

DESCRIPTION
  This routine notifies DIAG that data is ready to be sent without causing
  a context switch immediately.
  
  This eliminates concern that INTLOCK's may be violated from a context 
  switch.
  
RETURN VALUE
  None.

===========================================================================*/
  void diag_tx_notify (void);

/*===========================================================================

FUNCTION TYPE DIAG_EVENT_LISTENER

DESCRIPTION
  This function type is registered via diag_add_event_listener() or
  diag_add_event_range_listener() and is called by the DIAG task when an 
  event ID matching the registered value(s) is generated by DMSS/AMSS.
  
  This routine must not hold the memory very long.  If it is held too long
  it may hinder the performance of diagnostic services.  Memory is owned
  by the DIAG task and is freed when this function returns.
  
  The sequence number is incremented each time a new log is serviced.  This
  is useful to detect duplicated when more than one listener for the same
  type is used.
  
RETURN VALUE
  None.

===========================================================================*/
  typedef void (*diag_event_listener) (uint32 seq_num, 
                                       const diag_event_type * event, 
                                       void *param);

/*===========================================================================

FUNCTION TYPE DIAG_EVENT_LISTENER_EXT

DESCRIPTION
  This function type is registered via diag_add_event_listener_ext()
  and is called by the DIAG task when an event ID matching the
  registered value(s) is generated by DMSS/AMSS.
  
  This routine must not hold the memory very long.  If it is held too long
  it may hinder the performance of diagnostic services.  Memory is owned
  by the DIAG task and is freed when this function returns.
      
RETURN VALUE
  None.

===========================================================================*/
  typedef void (*diag_event_listener_ext) (const diag_event_type * event, 
                                       void *param);  
/*~ PARAM event POINTER */

/*===========================================================================

FUNCTION DIAG_ADD_EVENT_LISTENER

DESCRIPTION
  This routine registers a function to be called by the DIAG task when the
  specified event is generated by DMSS/AMSS.
  
  'param' will be passed unmodified to the registered function when called.
  
  Duplicate entries are allowed.  
    
RETURN VALUE
  TRUE - listener was successfully added to the listener table.
  FALSE - Listener was not added due to lack of resources.

===========================================================================*/
  boolean diag_add_event_listener (const unsigned int event_id,
                   const diag_event_listener listener,
                   void *param);

/*===========================================================================

FUNCTION DIAG_ADD_EVENT_LISTENER_EXT

DESCRIPTION
  This routine registers a function to be called by the DIAG task when any
  one of the events specified by the 'events' array is generated by DMSS/AMSS.
 
  'num_events' should be the number of events present in the 'events' array
 
  'param' will be passed unmodified to the registered function when called.
  
      
RETURN VALUE
  TRUE - listener was successfully added to the listener table.
  FALSE - Listener was not added due to lack of resources.

===========================================================================*/

  boolean diag_add_event_listener_ext (const uint16 *events, const unsigned int num_events, 
                         const diag_event_listener_ext listener, void *param);

/*~ FUNCTION diag_add_event_listener
     CLNTNAME diag_add_event_listener_on_modem */

/*===========================================================================

FUNCTION DIAG_REMOVE_EVENT_LISTENER

DESCRIPTION
  This routine unregisters a function from the event listener table.
  'listener' will no longer be called when the event ID is 
    
  'event_id', 'listener', and 'param' are used to match a registered 
  listener.
  
  If duplicate entries exist, only one is removed.

RETURN VALUE
  Boolean indicating the success of the operation.  If FALSE, the entry was
  not found in the listener table.

===========================================================================*/
  boolean diag_remove_event_listener (const unsigned int event_id,
                      const diag_event_listener listener,
                      void *param);

/*===========================================================================

FUNCTION DIAG_REMOVE_EVENT_LISTENER_EXT

DESCRIPTION
  This routine unregisters a function from the event listener table.
  'listener' will no longer be called when any of the events registered
  earlier by calling diag_add_event_listener_ext() is generated 
    
  'listener' is used to match a registered listener.
  'param' used with listener while adding the listener is not used to
   match listener.
  
  If duplicate entries exist, only one is removed.

RETURN VALUE
  Boolean indicating the success of the operation.  If FALSE, the entry was
  not found in the listener table.

===========================================================================*/
  boolean diag_remove_event_listener_ext (const diag_event_listener_ext listener);

/*~ FUNCTION diag_remove_event_listener
     CLNTNAME diag_remove_event_listener_on_modem */

/*===========================================================================

FUNCTION DIAG_REMOVE_EVENT_LISTENER_EXTP

DESCRIPTION
  This routine unregisters a function from the event listener table.
  'listener' with param 'param' will no longer be called when any of the events registered
  earlier by calling diag_add_event_listener_ext() is generated 
    
  'listener' and 'param' are used to match a registered listener.
  
  If duplicate entries exist, all are removed.

RETURN VALUE
  Boolean indicating the success of the operation.  If FALSE, the entry was
  not found in the listener table.

===========================================================================*/
  boolean diag_remove_event_listener_extp (const diag_event_listener_ext listener, void* param);


/*===========================================================================

FUNCTION TYPE DIAG_LOG_LISTENER

DESCRIPTION
  This function type is registered via diag_add_log_listener() or
  diag_listen_for_log_range() and is called by the DIAG task when an 
  log code matching the register value(s) is called.
  
  This routine must not hold the memory very long.  If it is held too long
  it may hinder the performance of diagnostic services.  Memory is owned
  by the DIAG task and is freed when this function returns.
  
  The sequence number is incremented each time a new log is serviced.  This
  is useful to detect duplicated when more than one listener for the same
  type is used.
  
RETURN VALUE
  None.

===========================================================================*/
  typedef void (*diag_log_listener) (uint32 seq_num, const byte *log, 
                                     unsigned int length, void *param);

/*===========================================================================

FUNCTION TYPE DIAG_LOG_LISTENER_EXT

DESCRIPTION
  This function type is registered via diag_log_event_listener_ext()
  and is called by the DIAG task when a log ID matching the
  registered value(s) is generated by DMSS/AMSS.
  
  This routine must not hold the memory very long.  If it is held too long
  it may hinder the performance of diagnostic services.  Memory is owned
  by the DIAG task and is freed when this function returns.
      
RETURN VALUE
  None.

===========================================================================*/

  typedef void (*diag_log_listener_ext) (const byte *log, 
                                     unsigned int length, void *param);  

/*~ PARAM log VARRAY LENGTH length */
/*===========================================================================

FUNCTION DIAG_ADD_LOG_LISTENER

DESCRIPTION
  This routine registers a function to be called when the specified log
  is generated by DMSS/AMSS.
  
  'param' will be passed unmodified to the registered function when called.
  
  Duplicate entries are allowed.  
    
RETURN VALUE
  TRUE - listener was successfully added to the listener table.
  FALSE - Listener was not added due to lack of resources.

===========================================================================*/
  boolean diag_add_log_listener (const unsigned int log_code,
                 const diag_log_listener listener,
                 void *param);

/*===========================================================================

FUNCTION DIAG_ADD_LOG_LISTENER_EXT

DESCRIPTION
  This routine registers a function to be called by the DIAG task when any
  one of the logs specified by the 'logs' array is generated by DMSS/AMSS.
 
  'num_logs' should be the number of logs present in the 'logs' array
 
  'param' will be passed unmodified to the registered function when called.
  
      
RETURN VALUE
  TRUE - listener was successfully added to the listener table.
  FALSE - Listener was not added due to lack of resources.

===========================================================================*/
  boolean diag_add_log_listener_ext (const uint16* logs, const unsigned int num_logs,
                       const diag_log_listener_ext listener, void *param);

/*~ FUNCTION diag_add_log_listener
     CLNTNAME diag_add_log_listener_on_modem */

/*===========================================================================

FUNCTION DIAG_REMOVE_LOG_LISTENER

DESCRIPTION
  This routine unregisters a function from the log listener table.
  'listener' will no longer be called when the log code is 
    
  'log_code', 'listener', and 'param' are used to match a registered 
  listener.
  
  If duplicate entries exist, only one is removed.

RETURN VALUE
  Boolean indicating the success of the operation.  If FALSE, the entry was
  not found in the listener table.

===========================================================================*/
  boolean diag_remove_log_listener (const unsigned int log_code,
                    const diag_log_listener listener,
                    void *param);

/*===========================================================================

FUNCTION DIAG_REMOVE_LOG_LISTENER_EXT

DESCRIPTION
  This routine unregisters a function from the log listener table.
  'listener' will no longer be called when any of the logs registered
  earlier by calling diag_add_log_listener_ext() is generated 
    
  'listener' is used to match a registered listener.
  'param' used with listener while adding the listener is not used to
   match listener.
  
  If duplicate entries exist, only one is removed.

RETURN VALUE
  Boolean indicating the success of the operation.  If FALSE, the entry was
  not found in the listener table.

===========================================================================*/
  boolean diag_remove_log_listener_ext (const diag_log_listener_ext listener);

/*~ FUNCTION diag_remove_log_listener
     CLNTNAME diag_remove_log_listener_on_modem */

	 
/*===========================================================================

FUNCTION DIAG_REMOVE_LOG_LISTENER_EXTP

DESCRIPTION
  This routine unregisters a function from the log listener table.
  'listener' with param 'param'  will no longer be called when any of the logs registered
  earlier by calling diag_add_log_listener_ext() is generated 
    
  'listener' and 'param' are used to match a registered listener.
  
  If duplicate entries exist, all are removed.

RETURN VALUE
  Boolean indicating the success of the operation.  If FALSE, the entry was
  not found in the listener table.

===========================================================================*/
  boolean diag_remove_log_listener_extp (const diag_log_listener_ext listener, void *param);

	 
/*===========================================================================

FUNCTION TYPE DIAG_F3_LISTENER

DESCRIPTION
  This function type is registered via diag_add_f3_listener() and is called 
  by the DIAG task when an SSID matching the registered value(s) is 
  generated by DMSS/AMSS.
  
  This routine must not hold the memory very long.  If it is held too long
  it may hinder the performance of diagnostic services.  Memory is owned
  by the DIAG task and is freed when this function returns.
  
  The sequence number is incremented each time a new F3 is serviced.  This
  is useful to detect duplicated when more than one listener for the same
  type is used.
  
RETURN VALUE
  None.

WARNING
  Client should not hold onto 'msg' since it will be freed by Diag task 
  when this function returns.
  
===========================================================================*/
  typedef void (*diag_f3_listener) (uint32 seq_num, 
                                    const diag_msg_type * msg, 
                                    void *param);
      

/*===========================================================================

FUNCTION TYPE DIAG_F3_LISTENER_EXT

DESCRIPTION
  This function type is registered via diag_add_f3_listener_ext() and is called 
  by the DIAG task when an SSID matching the registered value(s) is 
  generated by DMSS/AMSS.
  
  This routine must not hold the memory very long.  If it is held too long
  it may hinder the performance of diagnostic services.  Memory is owned
  by the DIAG task and is freed when this function returns.
  
RETURN VALUE
  None.

WARNING
  Client should not hold onto 'msg' since it will be freed by Diag task 
  when this function returns.
  
===========================================================================*/
  typedef void (*diag_f3_listener_ext) (const diag_msg_type * msg, 
                                          void *param);
      

/*===========================================================================

FUNCTION DIAG_ADD_F3_LISTENER

DESCRIPTION
  This routine registers a function to be called when the specified F3
  with matching mask is generated by DMSS/AMSS.
  
  'param' will be passed unmodified to the registered function when called.
  
  Duplicate entries are allowed.  
    
RETURN VALUE
  TRUE - listener was successfully added to the listener table.
  FALSE - Listener was not added due to lack of resources.

===========================================================================*/			
  boolean diag_add_f3_listener (const unsigned int ssid,
                                const unsigned int ss_mask,
                                const diag_f3_listener listener,
                                void *param);

			
/*===========================================================================

FUNCTION DIAG_ADD_F3_LISTENER_EXT

DESCRIPTION
	This routine registers a function to be called by the DIAG Task when any of the SSID+Mask pair in the keys array 
	matches with the F3 generated by DMSS/AMSS.

	'param' will be passed unmodified to the registered function when called.
	
	Duplicate entries are allowed.	
		
RETURN VALUE
	TRUE - listener was successfully added to the listener table.
	FALSE - Listener was not added due to lack of resources.

===========================================================================*/ 		

boolean diag_add_f3_listener_ext (f3_listener_ext_key_type *keys, 
									const unsigned int num_keys,
									const diag_f3_listener_ext listener,
									void *param);
								

/*===========================================================================

FUNCTION DIAG_REMOVE_F3_LISTENER

DESCRIPTION
  This routine unregisters a function from the F3 listener table.
  'listener' will no longer be called when the F3 SSID is 
  'ssid', 'ss_mask', 'listener', and 'param' are used to match a registered 
  listener.
  
  If duplicate entries exist, only one is removed.

RETURN VALUE
  Boolean indicating the success of the operation.  If FALSE, the entry was
  not found in the listener table.

===========================================================================*/               
  boolean diag_remove_f3_listener (const unsigned int ssid,
                                   const unsigned int ss_mask,
                                   const diag_f3_listener listener,
                                   void *param);


/*===========================================================================

FUNCTION DIAG_REMOVE_F3_LISTENER_EXT

DESCRIPTION
  This routine unregisters a function from the F3 listener table.
  'listener' will no longer be called for any F3 generated by DMSS/AMSS.
  
  If duplicate entries exist, all will be removed.

RETURN VALUE
  Boolean indicating the success of the operation.  If FALSE, the entry was
  not found in the listener table.

===========================================================================*/               
  boolean diag_remove_f3_listener_ext (const diag_f3_listener_ext listener, void* param);


/*===========================================================================

FUNCTION TYPE DIAG_CMD_RSP

DESCRIPTION
  This function type is provided by the caller when sending a request packet
  using diag_cmd_req().  After the request is processed, this function is
  called (if specified) with a pointer to the response packet and the handle
  returned from the corresponding diag_cmd_req() call.
  
  Memory is owned by the DIAG task and is freed when this function returns.
  
  'param' is the unmodified value from the corresponding diag_cmd_req() call.
  
RETURN VALUE
  None.

===========================================================================*/
  typedef void (*diag_cmd_rsp) (const byte *rsp, unsigned int length, 
                                void *param);

/*~ PARAM rsp VARRAY LENGTH length */

/*===========================================================================

FUNCTION DIAG_CMD_REQ

DESCRIPTION
  This routine injects the given request packet to the diagnostics service.
  The callback function, if specified, will be called with the response packet
  and 'param' (unmodified).
        
RETURN VALUE
  Boolean indicating success of enquing the command.

===========================================================================*/
  boolean diag_cmd_req (const byte *req, unsigned int length, 
                        const diag_cmd_rsp rsp_cb, void *param);
/*~ FUNCTION diag_cmd_req
     CLNTNAME diag_cmd_req_on_modem */
/*~ PARAM req VARRAY LENGTH length */


/* Sigs that are used by clients in IS95/IS2000/HDR targets.  These are 
   copied from diagi.h. */
#define DIAG_NV_CMD_SIG           0x00000004

/* TODO: Remove all references to this from DMSS as it has been deprecated */
#define DIAG_OFFLINE_COMPLETE_SIG 0

/* Signal that SIO is availabe to be opened for use. */
#define DIAG_COMM_OPEN_SIG        0x00000800    
                           
#define DIAG_COMM_OPEN_SMD_SIG    0x01000000    
#define DIAG_FWD_RX_SIG           0x02000000    

/* Signal to send TX traffic to SIO/SMD layer */
#define DIAG_DRAIN_TIMER_SIG      0x00200000    

#define DIAG_SIO_OPEN_SIG DIAG_COMM_OPEN_SIG

/*===========================================================================

FUNCTION DIAG_OTASP_UPDATE

DESCRIPTION
  This procedure tells diag to refresh its status cache from NV.

===========================================================================*/
  void diag_otasp_update (void);


/*===========================================================================

FUNCTION DIAG_TASK

DESCRIPTION
  This function contains the diagnostic task.  This task supports commun-
  ication with the Diagnostic Monitor, or other external diagnostic devices.
  Each packet received from the DM is processed and replied to, allowing the
  DM to perform configuration functions (NV item read and write, service programming),
  calibration functions (power adjustment table tweak), and remote handset
  emulation.

  This procedure does not return.

===========================================================================*/
  void diag_task (uint32 param);


/*==========================================================================

FUNCTION DIAG_EARLY_INIT

DESCRIPTION
  This function initializes the diagnostics message ,log and event services
  If the clients need to use diag apis before diag task is initialized this 
  function should be called

===========================================================================*/
  void diag_early_init(void);
 
/*==========================================================================

FUNCTION DIAG_WCDMA_WL1_FRAME_REG

DESCRIPTION
  This API will be used by WCDMA to register/unregister with Diag. The
  purpose of doing this is to avoid calling WCDMA function directly in
  Diag code.
 
PARAMETERS
  Valid pointer - Registers the cb
  NULL - Unregisters the cb
  
DEPENDENCIES
  None

RETURN VALUE
  Current preset mask index.

===========================================================================*/
void diag_wcdma_wl1_frame_reg(diag_wcdma_wl1_frame_cb_type ptr);

#ifdef __cplusplus
}
#endif
#endif              /* DIAG_H  */
