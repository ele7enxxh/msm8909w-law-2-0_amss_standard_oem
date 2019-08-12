/*---------------------------------------------------------------------------
   qmi_si_log.c - QMI Supported Interfaces Logging
---------------------------------------------------------------------------*/
/*****************************************************************************
  @file    qmi_si_log.c
  @brief   The QMI Supported Interfaces Logging
 
  DESCRIPTION
  This is the module to log the QMI Supported Interfaces to the
  QXDM log packet LOG_QMI_SUPPORTED_INTERFACES_C based on a
  QXDM subsystem request.

  ---------------------------------------------------------------------------
  Copyright (c) 2011-2015 Qualcomm Technologies Incorporated. All Rights Reserved. 
  QUALCOMM Proprietary and Confidential.
  ---------------------------------------------------------------------------
*******************************************************************************/
/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.mpss/3.5.c12.3/mproc/qmi/qmi_si/qmi_si_log.c#1 $

when         who     what, where, why
----------   ---     ---------------------------------------------------------- 
11/30/2011   aep     Adopt newer QMI SI interface
11/11/2011   dna     Initial Version
===========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <memory.h>
#include "comdef.h"

/* For mcsQcsiLsi Logging of QMI Supported Interfaces */
#include "diagpkt.h"
#include "diagcmd.h"
#include "msg.h"
#include "log.h"
#include "qmi_si.h"

#define BIT_SET(val, bit) (((val) & (1 << (bit))) ? 1 : 0)

#define E_SUCCESS   (0 )
#define E_FAILURE   (-1)
#define E_NO_MEMORY (-2)
/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/* -----------------------------------------------------------------------*//**
@brief
  MACRO definition identifying the version of the supported interfaces
  packet that is being logged 
*//* ------------------------------------------------------------------------*/
#define QMI_SIL_PKT_VERSION ( 1 )

/* -----------------------------------------------------------------------*//**
@brief
  MACRO definition identifying the size of the service fields in the 
        supported interfaces log packet
        service_id (1 byte) + exported_via_usb (1) + idl_major_version (1) + 
        idl_minor_version (1) + num_messages (2 bytes)
*//* ------------------------------------------------------------------------*/
#define QMI_SIL_SVC_HDR_SIZE_BYTES ( 6 )

/* -----------------------------------------------------------------------*//**
@brief
  MACRO definition identifying the size of each message in the 
        supported interfaces log packet
        message_id (2 bytes) + 3 num_optional_tlv fields (3 bytes) +
        total count of request, response and indication TLVs (1 byte each)
*//* ------------------------------------------------------------------------*/
#define QMI_SIL_MSG_SIZE_BYTES( total_num_tlvs ) (2 + 3 + total_num_tlvs)

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/* -----------------------------------------------------------------------*//**
@brief
  Log buffer state info
*//* ------------------------------------------------------------------------*/
typedef struct
{
  void *head;
  void *tail;
  uint32_t free_bytes;
} qmi_sil_lb_state_type;

/* -----------------------------------------------------------------------*//**
@brief
  Definition of the header of the log packet to log the supported
  interfaces and TLVs of all QMI services.  The main payload of the 
  log packet is filled in byte-by-byte so no C structure is defined.
*//* ------------------------------------------------------------------------*/

typedef PACKED struct PACKED_POST
{
  uint8_t         version; /*< Version of the log packet */
  uint8_t         total_num_svc; /*< Total number of services registered.  We may 
                                   use multiple log packets to log them all. */
  uint8_t         num_svc; /*< Number of services included */
  /** For each service the following structure is packed into the packet
      service_id: 1 byte
      num_messages: 2 bytes
      num_msgs occurrences of the following fields (for each service)
          message_id: 2 bytes
          num_optional_req_tlvs: 1 byte
          optional_req_tlv_ids: num_opt_req_tlvs bytes
          num_optional_rsp_tlvs: 1 byte
          optional_rsp_tlv_ids: num_opt_rsp_tlvs bytes
          num_optional_ind_tlvs: 1 byte
          optional_ind_tlv_ids: num_opt_ind_tlvs bytes
  */
} qmi_sil_log_pkt_hdr_type;

/* -----------------------------------------------------------------------*//**
@brief
  Registration for DIAG subsystem request to initiate logging of the 
  QMI supported interfaces packet.  Temporarily just re-use the HDR
  subsystem command 0xC0 which is very far from ever being used...
*//* ------------------------------------------------------------------------*/

#define QMI_SIL_DIAG_SUBSYS        (DIAG_SUBSYS_COREBSP)
#define QMI_SIL_DIAG_SUBSYS_CMD_ID (2147)


static PACKED void * qmi_sil_diag_req
(
  PACKED void   *req_pkt_ptr,
  uint16_t        pkt_len
);

static const diagpkt_user_table_entry_type qmi_sil_diag_subsys_reg_table[] =
{
  {QMI_SIL_DIAG_SUBSYS_CMD_ID, 
   QMI_SIL_DIAG_SUBSYS_CMD_ID, 
   qmi_sil_diag_req}
};

typedef PACKED struct PACKED_POST
{
  diagpkt_subsys_header_type   header;
  uint32 service_id;
} qmi_sil_req_pkt_type;

typedef struct {
  diagpkt_subsys_header_v2_type hdr;
  uint32                        command_status;
} qmi_sil_resp_pkt_type;


/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

/*===========================================================================
  FUNCTION  qmi_sil_lb_append
===========================================================================*/
/* -----------------------------------------------------------------------*//**
@brief
  Utility function to append a uint8_t value to the log buffer

@param[in]      lb_state_ptr  State of the log buffer
@param[in]      data          Pointer to data which needs to be appended.
@param[in]      size          Size of data to append

@retval    E_SUCCESS or E_NO_MEMORY
*//* ------------------------------------------------------------------------*/
static int qmi_sil_lb_append
(
  qmi_sil_lb_state_type *lb_state_ptr,
  const void *data,
  size_t size
)
{
  if( lb_state_ptr->free_bytes < size )
  {
    return E_NO_MEMORY;
  }

  memcpy(lb_state_ptr->tail, data, size);
  lb_state_ptr->tail = (uint8_t *)lb_state_ptr->tail + size;
  lb_state_ptr->free_bytes -= size;

  return E_SUCCESS;
}

/*===========================================================================
  FUNCTION  qmi_sil_lb_alloc
===========================================================================*/
/* -----------------------------------------------------------------------*//**
@brief
  Utility function to allocate a log buffer

@param[in]      lb_state_ptr  State of the log buffer
@param[in]      len           Number of bytes required in the log buffer
@param[in]      hdr_ptr       The header to be placed in the packet

@retval    E_SUCCESS or E_FAILURE
*//* ------------------------------------------------------------------------*/
static int qmi_sil_lb_alloc
(
  qmi_sil_lb_state_type *lb_state_ptr,
  uint32_t len,
  qmi_sil_log_pkt_hdr_type *hdr_ptr
)
{
  int ret_val = E_SUCCESS;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  lb_state_ptr->head = log_alloc( LOG_QMI_SUPPORTED_INTERFACES_C, len + sizeof(log_hdr_type));
  if ( lb_state_ptr->head != NULL )
  {
    lb_state_ptr->tail = (uint8_t *)lb_state_ptr->head + sizeof( log_hdr_type );
    lb_state_ptr->free_bytes = len;
    ret_val = qmi_sil_lb_append( lb_state_ptr, (const void *)hdr_ptr, sizeof(qmi_sil_log_pkt_hdr_type) );
  }
  else
  {
    ret_val = E_FAILURE;
  }

  return (ret_val);
}

/*===========================================================================
  FUNCTION  qmi_sil_lb_commit
===========================================================================*/
/* -----------------------------------------------------------------------*//**
@brief
  Utility function to commit the log buffer

@param[in]      lb_state_ptr  State of the log buffer
@param[in]      error_code error_code encountered while filling the packet
@retval    void
*//* ------------------------------------------------------------------------*/
static void qmi_sil_lb_commit
(
  qmi_sil_lb_state_type *lb_state_ptr,
  int error_code
)
{
  if (error_code == E_SUCCESS)
  {
    /* commit the Log Packet */
    log_commit ( lb_state_ptr->head );
  }
  else
  {
    log_free ( lb_state_ptr->head );
  }
}

/*===========================================================================
  FUNCTION  qmi_sil_count_tlv_bits
===========================================================================*/
/* -----------------------------------------------------------------------*//**
@brief
  Utility function to count the number of 1 bits in the TLV mask

@param[in]      tlv_list_ptr    The bit-mask of supported TLVs for a given
                                service's QMI request, response or indication

@retval    uint32_t value that is the number of 1 bits in the TLV bit-mask
*//* ------------------------------------------------------------------------*/
static uint8_t qmi_sil_count_tlv_bits
(
  const qmi_implemented_optionals *tlv_list_ptr
)
{
  uint8_t count = 0;
  uint32_t i, j;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ( tlv_list_ptr != NULL )
  {
    /* Message includes optional request TLVs */
    for ( i=0; i < tlv_list_ptr->array_size; i++ )
    {
      for ( j=0; j < 32; j++ )
      {
        count += BIT_SET(tlv_list_ptr->tlv_array[i], j);
      }
    }
  }
  return count;
}

/*===========================================================================
  FUNCTION  qmi_sil_compute_log_pkt_var_size
===========================================================================*/
/* -----------------------------------------------------------------------*//**
@brief
  Utility function to compute the required size for the variable length body 
  of the supported interfaces log packet

@param[in]      si_q_ptr      Pointer to the list of supported interfaces

@retval    uint32_t value that is the expected size of the log packet body, not
           including the log header (qmi_sil_log_pkt_hdr_type)
*//* ------------------------------------------------------------------------*/
static uint32_t qmi_sil_compute_log_pkt_var_size
(
  qmi_si_list_entry_type         *si_list_ptr
)
{
  qmi_si_list_entry_type         *si_entry_ptr = NULL;
  const qmi_implemented_messages *p_si;
  uint8_t                          tlv_count = 0;
  uint32_t                         i;
  uint32_t                         ret_val = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  si_entry_ptr = si_list_ptr;

  while (si_entry_ptr != NULL)
  {
    p_si = si_entry_ptr->si_tbl;
    ret_val += QMI_SIL_SVC_HDR_SIZE_BYTES;

    for ( i=0; i <= p_si->max_msg_id; i++ )
    {
      if ( p_si->message_array[i] != NULL )
      {  
        /* This message is supported by the service */
        tlv_count = qmi_sil_count_tlv_bits( p_si->message_array[i]->req );
        tlv_count += qmi_sil_count_tlv_bits( p_si->message_array[i]->resp );
        tlv_count += qmi_sil_count_tlv_bits( p_si->message_array[i]->ind );
        ret_val += QMI_SIL_MSG_SIZE_BYTES(tlv_count);
      }
    }

    si_entry_ptr = si_entry_ptr->next;
 }
  return ret_val;
}

/*===========================================================================
  FUNCTION  qmi_sil_fill_in_tlvs
===========================================================================*/
/* -----------------------------------------------------------------------*//**
@brief
  Utility function to fill in the supported TLV list in the log packet

@param[in]      lb_state_ptr    Pointer to the state of the log buffer
@param[in]      tlv_list_ptr    The bit-mask of supported TLVs for a given
                                buffer pointed to byLogPktTail

@retval    E_SUCCESS            Success
@retval    E_NO_MEMORY          The log buffer was too small
*//* ------------------------------------------------------------------------*/
static int qmi_sil_fill_in_tlvs
(
  qmi_sil_lb_state_type *lb_state_ptr,
  const qmi_implemented_optionals *tlv_list_ptr
)
{
  uint8_t     tlv_count = 0;
  uint32_t    i, j;
  uint8_t     tlv_id = 0x10; /* TLV mask bit 0 = 0x10 (first optional TLV) */
  int ret_val = E_SUCCESS;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /** Poor Mans Exception Handling */
  do
  {
    if ( tlv_list_ptr != NULL)
    {
      tlv_count = qmi_sil_count_tlv_bits( tlv_list_ptr );
    }
    else
    {
      tlv_count = 0;
    }

    ret_val = qmi_sil_lb_append( lb_state_ptr, (const void *)&tlv_count, sizeof(uint8_t) );
    if (ret_val != E_SUCCESS) { break; }
  
    if ( tlv_list_ptr != NULL)
    {
      /* TLV mask dword 0 bit 0 is TLV ID 0x10.  This is because the mask
         only needs to represent optional TLVs which are never numbered 0x00
         to 0x0F.  From there each bit lowest to highest represents the next 
         higher TLV ID.  If more than 32 optional TLVs are possible a second
         dword is added to the array so dword 1 bit 0 = TLV ID 0x30. */
      for ( i=0; i < tlv_list_ptr->array_size; i++ )
      {
        for ( j=0; j < 32; j++ )
        {
          if(BIT_SET(tlv_list_ptr->tlv_array[i], j))
          {
            ret_val = qmi_sil_lb_append( lb_state_ptr, (const void *)&tlv_id, sizeof(uint8_t));
            if (ret_val != E_SUCCESS) { break; }
          }
          tlv_id++;
        }
        if (ret_val != E_SUCCESS) { break; }
      }
      if (ret_val != E_SUCCESS) { break; }
    }
  } while (0);

  return ret_val;
}

/*===========================================================================
  FUNCTION  qmi_sil_fill_in_log_packet
===========================================================================*/
/* -----------------------------------------------------------------------*//**
@brief
  Utility function fills in the log packet with the details from the
  data registered by the various services.

@param[in]      lb_state_ptr  Pointer to the state of the log buffer
@param[in]      si_q_ptr      Pointer to the list of supported interfaces

@retval    E_SUCCESS          Success
@retval    E_NO_MEMORY        The log buffer was too small
*//* ------------------------------------------------------------------------*/
static int qmi_sil_fill_in_log_packet
(
  qmi_sil_lb_state_type    *lb_state_ptr,
  qmi_si_list_entry_type   *si_list_ptr
)
{
  qmi_si_list_entry_type           *si_entry_ptr = NULL;
  const qmi_implemented_messages   *p_si;
  uint16_t                         msg_count;
  int                              ret_val = E_SUCCESS;
  uint16_t                         msg_id;
  uint8_t                          exported_via_usb = 0;
  uint8_t                          service_id, major_vers, minor_vers;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  si_entry_ptr = si_list_ptr;

  while (si_entry_ptr != NULL)
  {
    /* Each sub packet starts with 
       service_id: 1 byte
       idl_major_ver: 1 byte
       idl_minor_ver: 1 byte
       num_messages: 2 bytes */
    service_id = (uint8_t)si_entry_ptr->service_id;
    major_vers = (uint8_t)si_entry_ptr->major_vers; 
    minor_vers = (uint8_t)si_entry_ptr->minor_vers;
    p_si       =          si_entry_ptr->si_tbl;

    ret_val = qmi_sil_lb_append( lb_state_ptr, (const void *)&service_id, sizeof(uint8_t) );
    if (ret_val != E_SUCCESS) { break; }
    /* Todo - Update the IDL compiler to add a zero placeholder and update qmi_implemented_messages 
       type such that p_si->exported_via_usb is used here instead of the local variable */
    ret_val = qmi_sil_lb_append( lb_state_ptr, (const void *)&exported_via_usb, sizeof(uint8_t) );
    if (ret_val != E_SUCCESS) { break; }
    ret_val = qmi_sil_lb_append( lb_state_ptr, (const void *)&major_vers, sizeof(uint8_t) );
    if (ret_val != E_SUCCESS) { break; }
    ret_val = qmi_sil_lb_append( lb_state_ptr, (const void *)&minor_vers, sizeof(uint8_t) );
    if (ret_val != E_SUCCESS) { break; }


    /* Need to count the number of messages supported by this service */
    msg_count = 0;
    for ( msg_id=0; msg_id <= p_si->max_msg_id; msg_id++ )
    {
      if ( p_si->message_array[msg_id] != NULL)
      {  
        msg_count++;
      }
    }

    ret_val = qmi_sil_lb_append( lb_state_ptr, (const void *)&msg_count, sizeof(uint16_t) );
    if (ret_val != E_SUCCESS) { break; }

    for ( msg_id=0; msg_id <= p_si->max_msg_id; msg_id++ )
    {
      if ( p_si->message_array[msg_id] != NULL )
      {  
        /* This message is supported by the service */

        /* Pack in the info for this message
            message_id: 2 bytes
            num_optional_req_tlvs: 1 byte
            optional_req_tlv_ids: num_opt_req_tlvs bytes
            num_optional_rsp_tlvs: 1 byte
            optional_rsp_tlv_ids: num_opt_rsp_tlvs bytes
            num_optional_ind_tlvs: 1 byte
            optional_ind_tlv_ids: num_opt_ind_tlvs bytes
        */
        ret_val = qmi_sil_lb_append( lb_state_ptr, (const void *)&msg_id, sizeof(uint16_t) );
        if (ret_val != E_SUCCESS) { break; }

        ret_val = qmi_sil_fill_in_tlvs(  lb_state_ptr,
                                         p_si->message_array[msg_id]->req );
        if (ret_val != E_SUCCESS) { break; }

        ret_val = qmi_sil_fill_in_tlvs(  lb_state_ptr,
                                         p_si->message_array[msg_id]->resp );
        if (ret_val != E_SUCCESS) { break; }

        ret_val = qmi_sil_fill_in_tlvs(  lb_state_ptr,
                                         p_si->message_array[msg_id]->ind );
        if (ret_val != E_SUCCESS) { break; }
      }
    }

    if (ret_val != E_SUCCESS) { break; }

    si_entry_ptr = si_entry_ptr->next;
  }
  return ret_val;
}

/*===========================================================================
  FUNCTION  qmi_sil_log
===========================================================================*/
/* -----------------------------------------------------------------------*//**
@brief
  Utility function to Log the QMI Supported Interfaces message.

  @param[in] service_id  The service ID to log (0 implies all)

*//* ------------------------------------------------------------------------*/
static void qmi_sil_log
(
  uint32_t service_id
)
{
  qmi_sil_lb_state_type    lb_state; /*< Log buffer state */
  qmi_sil_log_pkt_hdr_type log_header;
  int                      error_code = E_SUCCESS;
  qmi_si_list_entry_type   *si_list_ptr;
  qmi_si_list_entry_type   *si_entry_ptr;
  uint32_t                 log_pkt_size;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( log_status(LOG_QMI_SUPPORTED_INTERFACES_C) == TRUE )
  {
    si_entry_ptr = si_list_ptr = service_id == 0 ? 
      qmi_si_get_all_impl() : 
      qmi_si_get_service_id_impl(service_id, 0);

    log_header.num_svc = 0;
    while(si_entry_ptr)
    {
      log_header.num_svc++;
      si_entry_ptr = si_entry_ptr->next;
    }
    log_header.version = QMI_SIL_PKT_VERSION;

    /* For now we log all the service in one packet */
    log_header.total_num_svc = log_header.num_svc; 

    /* Log packet size is qmi_sil_log_pkt_hdr_type plus the size of each
       service's information */
    log_pkt_size = sizeof (log_header) + qmi_sil_compute_log_pkt_var_size( si_list_ptr );
    error_code = qmi_sil_lb_alloc( &lb_state, log_pkt_size, &log_header );

    if ( error_code == E_SUCCESS )
    {
      error_code = qmi_sil_fill_in_log_packet( &lb_state, si_list_ptr );

      qmi_sil_lb_commit( &lb_state, error_code );
    }
    else
    {
      MSG ( MSG_SSID_DFLT,
            MSG_LEGACY_ERROR,
            " Cannot allocate memory to log the supported interfaces packet " );
    }

    qmi_si_free_list(si_list_ptr);
  }
}


/*===========================================================================
  FUNCTION  qmi_sil_diag_req
===========================================================================*/
/* -----------------------------------------------------------------------*//**
@brief
  Function registered with DIAG to be called when the QMI supported 
  interfaces log packet is to be logged.
*//* ------------------------------------------------------------------------*/

static PACKED void * qmi_sil_diag_req
(
  PACKED void   *req_pkt_ptr,
  uint16_t        pkt_len
)
{
  diagpkt_subsys_cmd_code_type   subsys_code;
  uint32_t                       service_id = 0; /* Default All */
  qmi_sil_resp_pkt_type          *rsp_ptr = NULL;
  qmi_sil_req_pkt_type           *pkt = (qmi_sil_req_pkt_type *)req_pkt_ptr;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  subsys_code = diagpkt_subsys_get_cmd_code( req_pkt_ptr );

  if(subsys_code != QMI_SIL_DIAG_SUBSYS_CMD_ID)
  {
    return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt_ptr, pkt_len);
  }

  rsp_ptr = (qmi_sil_resp_pkt_type *) diagpkt_subsys_alloc_v2(
      QMI_SIL_DIAG_SUBSYS,
      subsys_code,
      sizeof(qmi_sil_resp_pkt_type)
      );

  if(!rsp_ptr)
  {
    return NULL;
  }

  rsp_ptr->command_status = 1;

  diagpkt_commit(rsp_ptr);

  if(pkt_len >= sizeof(qmi_sil_req_pkt_type))
  {
    service_id = pkt->service_id;
  }

  qmi_sil_log(service_id);

  return NULL;
}

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/

/*===========================================================================
  FUNCTION  qmi_sil_powerup_init
===========================================================================*/
/* -----------------------------------------------------------------------*//**
@brief
  Initialize QMI supported interfaces logging once on power up.

*//* ------------------------------------------------------------------------*/
void qmi_sil_powerup_init
(
  void
)                                
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DIAGPKT_DISPATCH_TABLE_REGISTER_V2 ( 
                        DIAG_SUBSYS_CMD_VER_2_F,
                        QMI_SIL_DIAG_SUBSYS, 
                        qmi_sil_diag_subsys_reg_table );
}
