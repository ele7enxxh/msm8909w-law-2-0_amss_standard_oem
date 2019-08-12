#ifndef MSGR_H
#define MSGR_H

/*!
  @file   msgr.h

  @brief  Message Router header file

  @detail
  Defines Message Router related types and interfaces.

*/
/*!
  @if msgr
  @mainpage Message Router
  @endif
  @ifnot msgr
  @page msgr_page Message Router
  @endif

  @ref umids

  <H1>UMID's</H1>
  <H2>What is it?</H2>
  A Unique Message ID (UMID) is assigned for each message that needs to be
  routable through the message router.

  The UMID is 32bit value that is built out of 4 x 8-bit segments - Technology,
  Module, Variant ID, Type, ID (in order of MSB to LSB, respectively), as shown below:
  
  <pre>
  +-------------------------------------------------------------+
  | Technology | Module   | Variant  | Message-Type | Message-ID |
  + ------------------------------------------------------------+
  </pre>
  - Technology - air interface or other technology
    - For example: COMMON, MM, WMX, LTE, etc.

  - Module - modules and spec-defined protocols that are particular to a technology
    - For example: MAC, SRCH, RLL, CP, FW, DS, CM, etc.

  - Type - Pre-defined message type
    - <code>SPR</code> - Supervisory message (e.g. watchdog loopback)
    - <code>CMD</code> - External Command
    - <code>CMDI</code> - Internal Command
    - <code>REQ</code> - External Requests
    - <code>REQI</code> - Internal Requests
    - <code>RSP</code> - External Response to a Command
    - <code>RSPI</code> - Internal Response to a Command
    - <code>IND</code> - External Indication
    - <code>INDI</code> - Internal Indication
    - <code>FLM</code> - OTA Forward Link Message
    - <code>OTA</code> - OTA Message
    - <code>DLM</code> - OTA Downward Link Message
    - <code>CNF</code> - External Confirmation Message
    - <code>CNFI</code> - Internal Confirmation Message
    - <code>TMR</code> - External Message caused by an expired timer
    - <code>TMRI</code> - Internal Message caused by an expired timer
    - <code>INTI</code> - Internal Interrupt

  - ID - User-defined message identifier
    - Defined by each individual module in their header file.

  <H2>How do I make one?</H2>
  The macro #MSGR_DEFINE_UMID is used in order to create a new UMID.  For
  example, suppose the follow is in a file called mymodule_umid.h:
  <code><pre>
  enum
  {
    #MSGR_DEFINE _UMID( MYTECH, MYMODULE, CMD, FOO, MYMODULE_ID_FOO,  mymodule_cmd_foo_s )
  };
  </pre></code>
  This will do 2 things:
  -# During compilation, the line will be replaced with the following.  The
    #MSGR_UMID macro simply concatenates the 4 values together into the 32-bit
    UMID as shown above.
  <code><pre>
  enum
  {
    MYTECH_MYMODULE_CMD_FOO = MSGR_UMID( MSGR_MYTECH_MYMODULE,
                                         MSGR_TYPE_CMD,
                                         MYMODULE_ID_FOO )
  };
  </pre></code>
  -# The the UMID will get added the MSGR internal registration tables

  After using #MSGR_DEFINE_UMID in a header file, software can use the symbol
  MYTECH_MYMODULE_CMD_FOO as a UMID in any of the MSGR API's.
  <H1></H1>
*/

/*===========================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/api/msgr.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/13/10   ejv     Update header file inclusions.
12/08/10   zm      Feature cleanup - SU API effort.
11/09/10   zm      Moved file into mcs/api folder - SU API effort.
06/21/10   gbs     fixed description of MSGR_UMID macro
06/2/10    gbs     Moved functions that are not qube specific into msgr.c
02/17/10   ejv     Remove inclusion of msgr_rex.h.
02/07/10   ejv     Remove deprecated MSGR functions.
11/06/09   ejv     Remove FEATURE_MSGR_DEPRECATED.
08/10/09   ejv     Wrap file in extern "C" as needed.
07/27/09   ejv     Remove qobject attachment references.
04/23/09   ejv     Add msgr_close_smd.
04/07/09   ejv     Remove FEATURE_MSGR_CS.
03/20/09   ejv     Add msgr_init_smd prototype.
03/19/09   enj     Add wrappers to expose test functionality
03/10/09   ejv     Add FEATURE_MSGR_DEPRECATED.
12/08/08   awj     Message router cleanup pass
11/14/08   ejv     Remove msgr_qube_send prototype.
10/31/08   ejv     Add FEATURE_MSGR_REX.
10/29/08   awj     Merged in from dev/posix/
10/14/08   ejv     Add msgr_client_type_is_valid.
09/05/08   awj     Move to OS independent message router API
06/19/08   ejv     Add msgr_get_umid_info.
05/16/08   ejv     Update for QDSP6 beta release.
01/25/08   sah     Add support for CS message router
01/14/08   enj     Split out Qube-specific API's
12/19/07   enj     MSGR API changes for attachments
11/29/07   enj     Added new API's for attachments and OS-independence
04/24/07   enj     Add MSGR_MAX_MSG_SIZE
04/23/07   enj     Rework UMID's to decouple from other components
04/10/07   VK      Changes to support DSM attachments
03/18/07   RC      Minor changes
02/25/07   RC      Initial release

===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <comdef.h>      /* Definition of basic types and macros */
//#include <stringl/stringl.h>
#include "msgr_umid.h"   /* UMID definitions */
#include "msgr_types.h"
#include "msgr_attach.h"
#include <IxErrno.h>

/*===========================================================================

                    EXTERNAL DEFINES

===========================================================================*/

#define  MSGR_SUCCESS              ((boolean) 1)
#define  MSGR_FAILURE              ((boolean) 0)

/* Identifies if client has been initialized or not */
#define  MSGR_CLIENT_ID_NULL       (0)

#define MSGR_OFFSETOF(type,member) (((char*)(&((type*)1)->member))-((char*)1))

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

void msgr_set_send_cb
(
  msgr_send_cb_t   cb   /*! Callback for receiving msgr_send events */
);

void msgr_init
(
  msgr_init_entry_s       *msgr_table,
  uint32                   num_msgrs,
  msgr_init_smd_entry_s   *smd_port_table,
  uint32                   num_smds,
  uint32                   index
);

void msgr_teardown( void ); 

errno_enum_type msgr_client_create
(
  msgr_client_t   *msgr_client_id
);

errno_enum_type msgr_client_add_mq
(
  const char                *name,
  msgr_client_t             *msgr_client_id,
  uint8                      pri,
  uint32                     max_msgs,
  uint32                     max_msg_size,
  msgr_id_t                 *queue_id
);

errno_enum_type msgr_client_add_queue
(
  const char      *name,
  msgr_client_t   *msgr_client_id,
  uint8            pri,
  uint32           queue_size,
  uint32           max_mallocs,
  msgr_id_t       *queue_id
);

errno_enum_type msgr_client_delete
(
  msgr_client_t             *msgr_client_id
);

void msgr_test_add_client
(
  msgr_client_t             *msgr_client_id
);

void msgr_test_remove_client
(
  msgr_client_t             *msgr_client_id
);

errno_enum_type msgr_query_umid
(
  msgr_tech_module_type      self,             //!< Registered TECH,MODULE
  msgr_client_t             *msgr_client_id,   //!< Message Client Handle
  msgr_umid_type             umid              //!< UMID which client desired
);

errno_enum_type msgr_register
(
  msgr_tech_module_type      self,             //!< Registering TECH,MODULE
  msgr_client_t             *msgr_client_id,   //!< Message Client Handle
  msgr_id_t                  id,               //!< Message Queue ID
  msgr_umid_type             umid              //!< UMID for which to register
);

errno_enum_type msgr_register_block
(
  msgr_tech_module_type      self,             //!< Registering TECH,MODULE
  msgr_client_t             *msgr_client_id,   //!< Message Client Handle
  msgr_id_t                  id,               //!< Message Queue ID
  msgr_umid_type            *umid_array,       //!< UMIDs for which to register
  uint32                     num_umids         //!< Number of UMIDs in array
);

errno_enum_type msgr_register_range
(
  msgr_tech_module_type      self,             //!< Registering TECH,MODULE
  msgr_client_t             *msgr_client_id,   //!< Message Client Handle
  msgr_id_t                  id,               //!< Message Queue ID
  msgr_umid_type             umid_from,        //!< UMID range, starting here
  msgr_umid_type             umid_to           //!<             ending here
);

errno_enum_type msgr_register_snoop
(
  msgr_tech_module_type      self,             //!< Registering TECH,MODULE
  msgr_client_t             *msgr_client_id,   //!< Message Client Handle
  msgr_id_t                  id                //!< Message Queue ID
);

errno_enum_type msgr_register_variant
(
  msgr_tech_module_type  self,                 //!< TECH,MODULE Performing Operation
  msgr_client_t         *msgr_client_id,       //!< Message Client Handle
  msgr_id_t              id,                   //!< Message Queue ID
  msgr_umid_type         umid,                 //!< UMID for which to register
  uint8                  variant               //!< Variant for which to register
);

errno_enum_type msgr_register_block_variant
(
  msgr_tech_module_type  self,                 //!< TECH,MODULE Performing Operation
  msgr_client_t         *msgr_client_id,       //!< Message Client Handle
  msgr_id_t              id,                   //!< Message Queue ID
  msgr_umid_type        *umid_array,           //!< UMID Array for which to register
  uint32                 num_umids,            //!< Number of UMIDs in array
  uint8                  variant               //!< Variant for which to register
);

errno_enum_type msgr_register_range_variant
(
  msgr_tech_module_type  self,                 //!< TECH,MODULE Performing Operation
  msgr_client_t         *msgr_client_id,       //!< Message Client Handle
  msgr_id_t              id,                   //!< Message Queue ID
  msgr_umid_type         umid_from,            //!< UMID range, starting here (from)
  msgr_umid_type         umid_to,              //!<             ending   here (to)
  uint8                  variant               //!< Variant for which to register
);

errno_enum_type msgr_deregister
(
  msgr_tech_module_type      self,             //!< Deregistering TECH,MODULE
  msgr_client_t             *msgr_client_id,   //!< Message Client Handle
  msgr_umid_type             umid              //!< UMID to deregister
);

errno_enum_type msgr_deregister_block
(
  msgr_tech_module_type      self,             //!< Deregistering TECH,MODULE
  msgr_client_t             *msgr_client_id,   //!< Message Client Handle
  msgr_umid_type            *umid,             //!< UMIDs to deregister   
  uint32                     num_umids         //!< Number of UMIDs in array
);

errno_enum_type msgr_deregister_range
(
  msgr_tech_module_type      self,             //!< Deregistering TECH,MODULE
  msgr_client_t             *msgr_client_id,   //!< Message Client Handle
  msgr_umid_type             umid_from,        //!< UMID range, starting here
  msgr_umid_type             umid_to           //!<             ending here 
);

errno_enum_type msgr_deregister_snoop
(
  msgr_tech_module_type      self,             //!< Deregistering TECH,MODULE
  msgr_client_t             *msgr_client_id    //!< Message Client Handle
);

errno_enum_type msgr_deregister_variant
(
  msgr_tech_module_type  self,                 //!< TECH,MODULE Performing Operation
  msgr_client_t         *msgr_client_id,       //!< Message Client Handle
  msgr_umid_type         umid,                 //!< UMID for which to register
  uint8                  variant               //!< Variant for which to deregister
);

errno_enum_type msgr_deregister_block_variant
(
  msgr_tech_module_type   self,             //!< Deregistering TECH,MODULE
  msgr_client_t          *msgr_client_id,   //!< Message Client Handle
  msgr_umid_type         *umid,             //!< UMIDs to deregister
  uint32                  num_umids,        //!< Number of UMIDs in array
  uint8                   variant           //!< Variant for which to deregister
);

errno_enum_type msgr_deregister_range_variant
(
  msgr_tech_module_type      self,             //!< Deregistering TECH,MODULE
  msgr_client_t             *msgr_client_id,   //!< Message Client Handle
  msgr_umid_type             umid_from,        //!< UMID range, starting here
  msgr_umid_type             umid_to,           //!<             ending here 
  uint8                      variant           //!< Variant for which to deregister
);

errno_enum_type msgr_send
(
  /*! Pointer to message to be sent. Note that the first member of the message
   *  that is pointed to by this pointer must be of msgr_hdr_struct_type type
   */
  msgr_hdr_struct_type      *msg_ptr,
  /*! Total message length (header and payload) in bytes */
  uint32                     msg_len
);

errno_enum_type msgr_send_direct
(
  msgr_client_t             *msgr_client_id,   //!< Message Client Handle
  msgr_hdr_struct_type      *msg_ptr,          //!< Msg Pointer Header,Payload
  uint32                     msg_size          //!< Msg Size    Header+Payload
);

errno_enum_type msgr_receive
(
  msgr_client_t             *msgr_client_id,   //!< Message Client Handle
  uint8                     *msg_buf,          //!< Buffer for incoming message
  uint32                     buf_size,         //!< Size of the buffer
  uint32                    *bytes_rcvd        //!< Amount of Data Received
);

errno_enum_type msgr_receive_nonblock
(
  msgr_client_t             *msgr_client_id,   //!< Message Client Handle
  uint8                     *msg_buf,          //!< Buffer for incoming message
  uint32                     buf_size,         //!< Size of the buffer
  uint32                    *bytes_rcvd        //!< Amount of Data Received
);

errno_enum_type msgr_peek
(
  msgr_client_t             *msgr_client_id,   //!< Message Client Handle
  msgr_hdr_s               **msg_pp,           //!< Pointer to place Address to FIFO tip
  msgr_fifo_handle_t        *fifo_handle,      //!< Return value of the FIFO's ID which was "peeked"
  boolean                    block             //!< Whether or not call should block
);

errno_enum_type msgr_pop
(
  msgr_client_t             *msgr_client_id,   //!< Message Client Handle
  msgr_fifo_handle_t         fifo_handle       //!< FIFO's ID from which to "pop"
);

void msgr_init_hdr
(
  msgr_hdr_struct_type      *hdr,
  msgr_tech_module_type      from,
  msgr_umid_type             umid
);

#define msgr_init_hdr_opts msgr_set_hdr_opts

void msgr_set_hdr_opts
(
  msgr_hdr_struct_type *hdr,
  msgr_hdr_opts_e       param_flag
);

void msgr_init_hdr_inst
(
  msgr_hdr_struct_type   *hdr,
  msgr_tech_module_type   from,
  msgr_umid_type          umid,
  uint8                   inst_id
);

void msgr_set_hdr_inst
(
  msgr_hdr_struct_type *hdr,
  uint8                 inst_id
);

void msgr_init_hdr_attach
(
  msgr_hdr_struct_type       *hdr,
  msgr_tech_module_type       from,
  msgr_umid_type              umid,
  msgr_priority_t             pri,
  uint8                       num
);

void msgr_set_hdr_attach
(
  msgr_hdr_struct_type *hdr,        /*!< Pointer to initialized msg header */
  uint8                 num_attach  /*!< Param to set */
);

void msgr_init_hdr_variant
(
  msgr_hdr_struct_type  *hdr,
  msgr_tech_module_type  from,
  msgr_umid_type         umid,
  uint8                  variant
);

void msgr_set_hdr_variant
(
  msgr_hdr_struct_type *hdr,
  uint8                 variant
);

errno_enum_type msgr_get_queue_stats
(
  msgr_id_t              id,             // Message Queue ID
  uint32                *usage,          // pointer to return max fifo size
  uint32                *count           // pointer to number of msgs in the queue 
);

/* Mask for variant bits */
#define MSGR_VARIANT_MASK          (MSGR_MAX_VARIANT << 13)
/* Zero out the variant bits in the umid */
#define MSGR_CLEAR_VARIANT(umid)   ((umid) &= ~MSGR_VARIANT_MASK)

/* Set the variant bits in the umid. Make sure to clear any existing variant first. */
#define MSGR_SET_VARIANT(umid, variant) ((umid) = (((umid) & ~MSGR_VARIANT_MASK) | (((variant) & MSGR_MAX_VARIANT) << 13)))
#define MSGR_GET_VARIANT(hdr)      MSGR_VARIANT_VAL((hdr)->id)

#define MSGR_GET_UMID(hdr)         ((hdr)->id)
#define MSGR_GET_INST(hdr)         ((hdr)->inst_id)
#define MSGR_GET_NUM_ATTACH(hdr)   ((hdr)->num_attach)
#define MSGR_GET_PAYLOAD(hdr)      ((void*)(((msgr_attach_s*)(hdr + 1)) + (hdr)->num_attach))
#define MSGR_GET_ATTACH_TYPE(att)  ((att)->type)

#define msgr_get_umid(hdr)         ((hdr)->id)
#define msgr_get_num_attach(hdr)   ((hdr)->num_attach)
#define msgr_get_payload(hdr)      ((void*)(((msgr_attach_s*)(hdr + 1)) + (hdr)->num_attach))
#define msgr_get_attach_type(att)  ((att)->type)

#define msgr_get_attach(hdr,n)     (((hdr)->num_attach > n) ? (((msgr_attach_s*)(hdr + 1)) + n) : NULL )

#define msgr_clear_attach(att)                              \
  do {                                                      \
    memset( (att), 0, sizeof(msgr_attach_s) );              \
    (att)->type = MSGR_ATTACH_NONE;                         \
  } while(0)

#define msgr_copy_attach(att1,att2)                         \
  do {                                                      \
    memscpy( &(att1)->data,                                 \
            sizeof(msgr_attach_data_u),                     \
            &(att2)->data,                                  \
            sizeof(msgr_attach_data_u) );                   \
    (att1)->type = (att2)->type;                            \
  } while(0)


#define msgr_set_dsm_attach(att,p)                          \
  do {                                                      \
    (att)->type = MSGR_ATTACH_DSM;                          \
    (att)->data.dsm.dsm_ptr = (p);                          \
    (att)->data.dsm.meta_info_incl = FALSE;                 \
    (att)->data.dsm.meta_info_ptr  = 0;                     \
  } while(0)

#define msgr_get_dsm_attach(att, dsm_ptr_ptr)               \
  do {                                                      \
    if( (att)->type == MSGR_ATTACH_DSM )                    \
    {                                                       \
      *(dsm_ptr_ptr) = (att)->data.dsm.dsm_ptr;             \
    }                                                       \
    else                                                    \
    {                                                       \
      *(dsm_ptr_ptr) = NULL;                                \
    }                                                       \
  } while(0)

#define msgr_set_general_attach(att,p,s)                    \
  do {                                                      \
    (att)->type = MSGR_ATTACH_GENERAL;                      \
    (att)->data.general.ptr  = (p);                         \
    (att)->data.general.size = (s);                         \
  } while(0)

#define msgr_get_general_attach(att, ptr_ptr, size_ptr)     \
  do {                                                      \
    if( (att)->type == MSGR_ATTACH_GENERAL )                \
    {                                                       \
      *(ptr_ptr)  = (att)->data.general.ptr;                \
      *(size_ptr) = (att)->data.general.size;               \
    }                                                       \
    else                                                    \
    {                                                       \
      *(ptr_ptr)  = NULL;                                   \
      *(size_ptr) = 0;                                      \
    }                                                       \
  } while(0)

void msgr_init_loopback
(
  msgr_spr_loopback_struct_type *loopback,
  msgr_tech_module_type          src,
  msgr_umid_type                 umid,
  uint32                         reply_token
);

errno_enum_type msgr_loopback_reply
(
  msgr_spr_loopback_struct_type *loopback,
  msgr_tech_module_type          src
);

/* Get a pointer to the UMID name */
const char * msgr_get_umid_name_ext
(
  msgr_hdr_struct_type          *msg,
  void                          *userdata
);

/* Get a pointer to the Tech/Module name */
const char * msgr_get_module_name
(
  msgr_tech_module_type          module
);

/* Get a pointer to the UMID name */
const char * msgr_get_umid_name
(
  msgr_umid_type                 umid
);

/* Get a UMID from its name*/
uint32 msgr_get_umid_from_name
(
  const char* name
);

/* Get the registered module and client for a given UMID */
void msgr_get_umid_info
(
  msgr_umid_type                 umid,
  msgr_tech_module_type         *mod_ptr,
  msgr_client_t                 *client_ptr
);

/* Allow checking for a valid client type */
boolean msgr_client_type_is_valid
(
  msgr_client_t                  msgr_client_id
);

/* Allow access to MSGR SMD initialization */
void msgr_init_smd
(
  msgr_init_entry_s       *msgr_table,
  uint32                   num_msgrs,
  msgr_init_smd_entry_s   *smd_port_table,
  uint32                   num_smds,
  uint32                   index
);

/* Allow access to closing out SMD ports */
errno_enum_type msgr_close_smd
(
  msgr_init_smd_entry_s   *smd_port_table,
  uint32                   num_smds
);

#ifdef __cplusplus
}
#endif

#endif /* MSGR_H */

