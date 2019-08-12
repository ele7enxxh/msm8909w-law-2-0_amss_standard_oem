/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                 D S _ Q M I _ W D S _ P B M . C
DESCRIPTION
  The QMI Data Services Call History - Phone Book Manager (PBM) interface 
  Source File.

EXTERNALIZED FUNCTIONS
  qmi_pbm_add_record()
  qmi_pbm_get_num_records()
  qmi_pbm_get_list()
  qmi_pbm_get_var_len_list()
  qmi_pbm_get_record_by_id()
  qmi_pbm_clear_history()
  
Copyright (c) 2008-13 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/qmidata/src/ds_qmi_wds_pbm.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $  

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/22/13   gk      Made sure that stray phonebook records are ignored 
                    since ATCoP is also writing tothe same phonebook for 
                    DUN calls in 3GPP mode
04/05/12   sn      Fixed write record failure by correcting data_buf_len
02/17/12   sb      Fixed Compiler and KW Warnings.
08/21/08   bd      Created module
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "customer.h"

#ifndef FEATURE_PBM_TASK
#error "FEATURE_PBM_TASK must be enabled for QMI_WDS call history"
#endif /* FEATURE_PBM_TASK */

#include "amssassert.h"
#include "pbmlib.h"

#include "ds_qmi_defs.h"
#include "ds_qmi_wds_pbm.h"
#include "ds_Utils_DebugMsg.h"

#include <stringl/stringl.h>


/*===========================================================================

                       CONSTANT DEFINITIONS AND MACROS

===========================================================================*/

#define ALIGNMENT 4       /* Keep fields aligned on this boundry. */
#define ALIGNED_SIZE(x)   (ALIGNMENT * (((x) + ALIGNMENT - 1)/ALIGNMENT))

#define QMI_PBM_HEADER_EFS_LEN      (ALIGNED_SIZE(sizeof(pbm_field_s_type)))
#define QMI_PBM_HIST_ENTRY_MAX_EFS_LEN   (ALIGNED_SIZE(sizeof(qmi_pbm_callhist_s_type)))
#define QMI_PBM_RECORD_MAX_EFS_LEN  (QMI_PBM_HIST_ENTRY_MAX_EFS_LEN + QMI_PBM_HEADER_EFS_LEN)

#define QMI_PBM_PB_ID  PBM_DS_LD
#define QMI_PBM_PB_CAT PBM_CAT_NONE
/* Indexing starts @ 1 for PBM locations */
#define QMI_PBM_LOCATION_START (1)

#define QMI_PBM_HIST_ENTRY_FIELD_INDEX   0

#define QMI_PBM_NUM_PB_FIELDS         1

#define QMI_PBM_UNIQUE_ID_UNINITIALIZED (0xFFFF)

/* Shortcut macro for qmi_pbm_get_list_packed that does everything 
 * needed to copy an item into qmi_resp */
#define QMI_PBM_CHECK_THEN_COPY(qmi_resp_ptr, resp_size, resp_used, item_to_copy) \
  if (resp_size < (sizeof(item_to_copy) + resp_used)) {                     \
    ret = QMI_ERR_NO_MEMORY_V01;                                                \
    break;                                                                  \
  }                                                                         \
  memscpy(qmi_resp_ptr, sizeof(item_to_copy), (char *) &(item_to_copy), sizeof(item_to_copy));     \
  qmi_resp_ptr += sizeof(item_to_copy);                                     \
  resp_used += sizeof(item_to_copy);

#define QMI_PBM_DTMF_NULL      0
#define QMI_PBM_DTMF_ZERO     10
#define QMI_PBM_DTMF_STAR     11
#define QMI_PBM_DTMF_POUND    12
#define QMI_PBM_DTMF_UNKNOWN  13

#ifndef MAX_UINT32
#define MAX_UINT32 0xffffffff
#endif



/*===========================================================================

                            LOCAL GLOBAL VARIABLES

===========================================================================*/

static uint16 qmi_pbm_unique_id = QMI_PBM_UNIQUE_ID_UNINITIALIZED;

static const char qmi_pbm_dtmf_to_ascii_lut[] =
{
  '\0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '*', '#', '?', '?', '?'
};


/*===========================================================================

                            FORWARD DECLARATIONS

===========================================================================*/


static qmi_error_type_v01 qmi_pbm_get_record_by_rec_id
(
  uint16                   rec_id,
  qmi_pbm_callhist_s_type *qmi_data
);

static void copy_qmi_to_pbm
(
  const qmi_pbm_callhist_s_type *qmi_data,
  uint8   *pbm_data_buf,
  uint16  *new_id,
  uint16  *data_buf_len
);

static qmi_error_type_v01 copy_pbm_to_qmi
(
  const uint8 *pbm_data_buf,
  qmi_pbm_callhist_s_type *qmi_data
);

static qmi_error_type_v01 qmi_pbm_write_record
(
  uint8   *pbm_data_buf,
  uint16   data_buf_len
);

static void * qmi_pbm_get_field_data_ptr
( 
  const void *field_buf,
  int field_num
);

static uint16 qmi_pbm_get_field_len
( 
  const void *field_buf,
  int field_num
);

static uint16 qmi_pbm_get_next_unique_id
( 
  void
);

static void qmi_pbm_init_unique_id
( 
  void
);

static uint8 qmi_pbm_conv_ascii_to_dtmf
(
  char            *target,
  uint8           len
);

static void qmi_pbm_conv_dtmf_to_ascii
(
  char            *target,
  uint8           len
);

static inline uint8 qmi_pbm_ascii_char_to_dtmf
(
  char ascii_val
);



/*===========================================================================

                        EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/


/*===========================================================================
  FUNCTION QMI_PBM_ADD_RECORD()

  DESCRIPTION
    Creates and writes a new PBM data services call history entry.  If 
    new_id is not NULL, it will contain the new unique ID.  If the write
    fails, and new_id is not NULL, new_id will contain the unique ID that
    would have been used had the write not failed.  This unique ID will not
    be used by the next write.

  PARAMETERS
    qmi_data : contains all data to write into PBM
    rec_id   : (out) record ID of the newly inserted entry

  RETURN VALUE
    QMI_ERR_NONE on success, QMI_ERR_INTERNAL on write failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None  
===========================================================================*/
qmi_error_type_v01 qmi_pbm_add_record
(
  const qmi_pbm_callhist_s_type *qmi_data,
  uint16 *new_id
)
{
  uint8  pbm_data_buf[QMI_PBM_RECORD_MAX_EFS_LEN];
  uint16 data_buf_len = 0;

  memset( &(pbm_data_buf[0]), 0, sizeof(pbm_data_buf) );

  copy_qmi_to_pbm(qmi_data, pbm_data_buf, new_id, &data_buf_len);

  return qmi_pbm_write_record(pbm_data_buf, data_buf_len);
}

/*===========================================================================
  FUNCTION QMI_PBM_GET_NUM_RECORDS()

  DESCRIPTION
    Returns the number of data services call history entries currently stored
    in PBM.

  PARAMETERS
    None

  RETURN VALUE
    Number of DS call history records

  DEPENDENCIES
    None

  SIDE EFFECTS
    None  
===========================================================================*/
uint16 qmi_pbm_get_num_records
(
  void
)
{
  return pbm_get_num_recs(QMI_PBM_PB_ID);
}

/*===========================================================================
  FUNCTION QMI_PBM_GET_LIST()

  DESCRIPTION
    Gathers up to the newest ary_size entries in the call history store and
    places them in qmi_data_ary.  The parameter qmi_data_ary should already
    be allocated.  If rec_id_ary is not null, this will contain the specific
    unique ID numbers for each record, in the same order (newest to oldest)
    as qmi_data_ary.  To get an array containing all listings, ary_size 
    should be at least qmi_pbm_get_num_records(), or statically 
    QMI_PBM_MAX_PB_ENTRIES.

  PARAMETERS
    qmi_data_ary : (out) buf to store array of QMI call history structs
    rec_id_ary   : (out) either null or a preallocated array to store the
                   record IDs
    ary_size     : (in)  number of entries that can fit in qmi_data_ary 
                   and rec_id_ary (should be the same for both)

  RETURN VALUE
    Number of records read (0 on error or if the store is empty)

  DEPENDENCIES
    None

  SIDE EFFECTS
    None  
===========================================================================*/
uint16 qmi_pbm_get_list
(
  qmi_pbm_callhist_s_type *qmi_data_ary,
  uint16 *rec_id_ary,
  uint16 ary_size
)
{
  uint16 i,j=0;
  uint16 rec_id;
  pbm_return_type pbm_ret;
  uint16 pbm_size;
  qmi_error_type_v01 qmi_err;

  ASSERT(qmi_data_ary);

  /* Initialize a search for all DS call history entries */
  pbm_ret = pbm_enum_rec_init(QMI_PBM_PB_ID, QMI_PBM_PB_CAT, PBM_FIELD_NONE,
                              NULL, 0, 0);
  if (PBM_SUCCESS != pbm_ret)
  {
    LOG_MSG_ERROR_1("PBM DS call history listing failed during init (pbm_ret = %d)",
                    pbm_ret);
    return 0;
  }

  pbm_size = qmi_pbm_get_num_records();
  for (i = 0; i < MIN(ary_size, pbm_size); i++)
  {
    pbm_ret = pbm_enum_next_rec_id(&rec_id);
    if (PBM_SUCCESS != pbm_ret)
    {
      LOG_MSG_ERROR_1("PBM DS call history listing failed during next_rec_id (pbm_ret = %d)",
                      pbm_ret);
      break;
    }

    if (QMI_ERR_NONE_V01!= (qmi_err = qmi_pbm_get_record_by_rec_id(rec_id, &(qmi_data_ary[j]))))
    {
      /* If the retrieved record has more than 1 field ignore it as others might have written into it*/
      if(QMI_ERR_UNKNOWN_V01== qmi_err)
      {
        continue;
      }
      else
    {
      break;
    }
    }

    if (rec_id_ary != NULL)
    {
      rec_id_ary[j++] = rec_id;
    }
  }

  return j;
}

/*===========================================================================
  FUNCTION QMI_PBM_GET_VAR_LEN_LIST()

  DESCRIPTION
    Retrieves data from PBM and places it in a response buffer intended for
    QMI consumption. 

  PARAMETERS
    resp_msg     : (out) preallocated buffer for QMI response
    resp_size    : (in)  size (in bytes) of the qmi_resp buffer
    field_mask   : (in)  bitmask describing what data to include (see: QMI_PBM_MASK_ALL)

  RETURN VALUE
    QMI_ERR_NONE_V01 on success, QMI_ERR_NO_MEMORY_V01 if the buffer isn't large 
    enough to hold the data, QMI_ERR_INTERNAL_V01 on PBM failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None  
===========================================================================*/
qmi_error_type_v01 qmi_pbm_get_var_len_list
(
  wds_call_history_list_resp_msg_v01* resp_msg,
  uint16  resp_size,
  uint16  field_mask
)
{
  uint16                    rec_id;
  pbm_return_type           pbm_ret;
  qmi_pbm_callhist_s_type   callhist_data;
  qmi_error_type_v01        ret,qmi_err;

  ret = QMI_ERR_NONE_V01;
  LOG_MSG_INFO2_1("Initializing PBM packed list retrieval for mask 0x%x",
                  field_mask);

  /* To make sure we don't dereference invalid qmi_resp_buf pointed memory when resp_size is 0 */
  if(0 == resp_size || resp_msg == NULL)
  {
    LOG_MSG_ERROR_0("PBM DS call history listing failed. resp_size = 0 or msg = NULL passed");
    return QMI_ERR_INTERNAL_V01;
  }

  /* Initialize a search for all DS call history entries */
  pbm_ret = pbm_enum_rec_init(QMI_PBM_PB_ID, QMI_PBM_PB_CAT, PBM_FIELD_NONE,
                              NULL, 0, 0);
  if (pbm_ret != PBM_SUCCESS)
  {
    LOG_MSG_ERROR_1("PBM DS call history listing failed during init (pbm_ret = %d)",
                    pbm_ret);
    return QMI_ERR_INTERNAL_V01;
  }

  while ((pbm_ret = pbm_enum_next_rec_id(&rec_id)) != PBM_EOF)
  {
    /* Still have to check pbm_ret for error conditions */
    if (pbm_ret != PBM_SUCCESS)
    {
      LOG_MSG_ERROR_1("PBM DS call history listing failed during next_rec_id (pbm_ret = %d)",
                      pbm_ret);
      /* No error if this is an empty phonebook */
      if (0 != qmi_pbm_get_num_records() || PBM_ERROR_NOT_FOUND != pbm_ret)
      {
        ret = QMI_ERR_INTERNAL_V01;
      }
      break;
    }
    if(resp_msg->full_call_history.full_call_history_len >= QMI_WDS_MAX_CALL_HISTORY_V01)
    {
      LOG_MSG_ERROR_0("full_call_history_len max leangth reached");
      break;
    }
    if (QMI_ERR_NONE_V01!= (qmi_err = qmi_pbm_get_record_by_rec_id(rec_id, &callhist_data)))
    {
      /* If the retrieved record has more than 1 field ignore it as others might have written into it*/
      if(QMI_ERR_UNKNOWN_V01== qmi_err)
      {
        continue;
      }
      else
      {
        ret = QMI_ERR_INTERNAL_V01;
        break;
      }
    }
      /* Copy individual fields in order */
    if (field_mask & QMI_PBM_MASK_UNIQUE_ID)
    {
      resp_msg->full_call_history.full_call_history[resp_msg->full_call_history.\
                    full_call_history_len].call_record_id = callhist_data.unique_id;
      /* Current implementation uses either MASK_ALL or MASK_UNIQUE_ID - so checking for
      * unique ID here will speed things up a bit by not having to do all the other checks */
      if (QMI_PBM_MASK_UNIQUE_ID == field_mask)
      {
        resp_msg->full_call_history.full_call_history_len++;
        continue;
      }
    }
    if (field_mask & QMI_PBM_MASK_SOURCE)
    {
      resp_msg->full_call_history.full_call_history[resp_msg->full_call_history.\
                   full_call_history_len].call_record.call_type = 
                                                    callhist_data.source;
    }
    if (field_mask & QMI_PBM_MASK_TECH_TYPE)
    {
      resp_msg->full_call_history.full_call_history[resp_msg->full_call_history.\
                   full_call_history_len].call_record.call_data_bearer = 
                                                    callhist_data.tech_type_e;
    }
    if (field_mask & QMI_PBM_MASK_DATE)
    {
      resp_msg->full_call_history.full_call_history[resp_msg->full_call_history.\
                   full_call_history_len].call_record.call_timestamp =
                                                    callhist_data.date;
    }
    if (field_mask & QMI_PBM_MASK_IPADDR)
    {
      resp_msg->full_call_history.full_call_history[resp_msg->full_call_history.\
                   full_call_history_len].call_record.call_ip_addr = 
                                                    callhist_data.ipv4_addr;
    }
    if (field_mask & QMI_PBM_MASK_DURATION)
    {
      resp_msg->full_call_history.full_call_history[resp_msg->full_call_history.\
                   full_call_history_len].call_record.call_duration_total = 
                                                    callhist_data.call_duration;
    }
    if (field_mask & QMI_PBM_MASK_DURATION_ACTIVE)
    {
      resp_msg->full_call_history.full_call_history[resp_msg->full_call_history.\
                   full_call_history_len].call_record.call_duration_active = 
                                                    callhist_data.call_duration_active;
    }
    if (field_mask & QMI_PBM_MASK_RX_BYTES)
    {
      resp_msg->full_call_history.full_call_history[resp_msg->full_call_history.\
                   full_call_history_len].call_record.call_rx_ok_bytes = 
                                                    callhist_data.rx_bytes;
    }
    if (field_mask & QMI_PBM_MASK_TX_BYTES)
    {
      resp_msg->full_call_history.full_call_history[resp_msg->full_call_history.\
                   full_call_history_len].call_record.call_tx_ok_bytes= 
                                                    callhist_data.tx_bytes;
    }
    if (field_mask & QMI_PBM_MASK_CALL_ERR)
    {
      resp_msg->full_call_history.full_call_history[resp_msg->full_call_history.\
                   full_call_history_len].call_record.call_end_reason = 
                                                    callhist_data.call_err_e;
    }
  

    /* Special case: phone number is a variable-length field */
    if (field_mask & QMI_PBM_MASK_PHONE)
    {
      if (callhist_data.phone_num_len > QMI_WDS_MAX_PHONE_NUMBER_LEN_V01)
      {
        callhist_data.phone_num_len = QMI_WDS_MAX_PHONE_NUMBER_LEN_V01;
      }
      memscpy((void*)resp_msg->full_call_history.full_call_history[resp_msg->full_call_history.\
                   full_call_history_len].call_record.call_phone_num, 
              sizeof(resp_msg->full_call_history.full_call_history[resp_msg->full_call_history.\
                   full_call_history_len].call_record.call_phone_num),
              (void *)(callhist_data.phone_num), 
              callhist_data.phone_num_len);
    }
    resp_msg->full_call_history.full_call_history_len++;
  }
  if(resp_msg->full_call_history.full_call_history_len)
  {
    resp_msg->full_call_history_valid = TRUE;
  }
  return ret;
}

/*===========================================================================
  FUNCTION QMI_PBM_GET_RECORD_BY_ID()

  DESCRIPTION
    Reads a record from PBM into a QMI data structure using the QMI PBM
    unique identifier.

  PARAMETERS
    unique_id : unique ID of record to look up
    qmi_data  : pointer to preallocated call history struct which will be
                populated with the newly read data

  RETURN VALUE
    QMI_ERR_NONE_V01 on success, QMI_ERR_INTERNAL_V01 on read failure, 
    QMI_ERR_NO_ENTRY_V01 if we don't have any data for that ID (may have been
    shifted out), QMI_ERR_INVALID_INDEX_V01 if unique_id is an ID that hasn't
    been used yet

  DEPENDENCIES
    None

  SIDE EFFECTS
    None  
===========================================================================*/
qmi_error_type_v01 qmi_pbm_get_record_by_id
(
  uint16                   unique_id,
  qmi_pbm_callhist_s_type *qmi_data
)
{
  uint16          location;
  uint16          rec_id;
  uint8           ret;

  /* Find out what our current unique ID is if we haven't already */
  if (QMI_PBM_UNIQUE_ID_UNINITIALIZED == qmi_pbm_unique_id)
  {
    qmi_pbm_init_unique_id();
  }

  if (QMI_PBM_UNIQUE_ID_UNINITIALIZED == qmi_pbm_unique_id ||
      QMI_PBM_UNIQUE_ID_UNINITIALIZED == unique_id)
  {
    return QMI_ERR_INVALID_INDEX_V01;
  }

  /* Handle 16-bit unsigned wraparound while avoiding the special
   * value QMI_PBM_UNIQUE_ID_UNINITIALIZED == 0xFFFF */
  if (qmi_pbm_unique_id < (QMI_PBM_MAX_PB_ENTRIES - 1) &&
      unique_id > (QMI_PBM_UNIQUE_ID_UNINITIALIZED - (QMI_PBM_MAX_PB_ENTRIES - qmi_pbm_unique_id)))
  {
    location = qmi_pbm_unique_id + (QMI_PBM_UNIQUE_ID_UNINITIALIZED - unique_id);
  }
  else if (unique_id > qmi_pbm_unique_id)
  {
    /* Not a wraparound condition, and trying to retrieve a unique ID that hasn't
     * been used yet: invalid index */
    return QMI_ERR_INVALID_INDEX_V01;
  }
  else
  {
    /* Determine the PBM record ID using the unique ID */
    location = qmi_pbm_unique_id - unique_id;
  }

  location += QMI_PBM_LOCATION_START;
  if (location > QMI_PBM_MAX_PB_ENTRIES || location < QMI_PBM_LOCATION_START)
  {
    /* This means the record has already been shifted out of our circular buffer */
    return QMI_ERR_NO_ENTRY_V01;
  }

  rec_id = pbm_location_to_record_id(QMI_PBM_PB_ID, location);

  LOG_MSG_INFO2_2("Initialized read of record ID %d (unique ID %d)",
                  rec_id, unique_id);
  if(QMI_ERR_UNKNOWN_V01== (ret = qmi_pbm_get_record_by_rec_id(rec_id, qmi_data)))
  {
    location += 1;
    rec_id = pbm_location_to_record_id(QMI_PBM_PB_ID, location);
  }
  else
  {
    return ret;
  }
  return qmi_pbm_get_record_by_rec_id(rec_id, qmi_data);
}


/*===========================================================================
  FUNCTION QMI_PBM_CLEAR_HISTORY()

  DESCRIPTION
    Deletes all records in the PBM data services call history store, and 
    resets the unique_id for the next entry back to 0.

  PARAMETERS
    None

  RETURN VALUE
    QMI_ERR_NONE_V01 on success, QMI_ERR_INTERNAL_V01 on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None  
===========================================================================*/
qmi_error_type_v01 qmi_pbm_clear_history
(
  void
)
{
  if (PBM_SUCCESS == pbm_clear_phonebook(QMI_PBM_PB_ID))
  {
    /* Success, reset ID */
    qmi_pbm_unique_id = QMI_PBM_UNIQUE_ID_UNINITIALIZED;
    return QMI_ERR_NONE_V01;
  }
  /* pbm_clear_phonebook failed, return internal error */
  return QMI_ERR_INTERNAL_V01;
}


/*===========================================================================

                        INTERNAL FUNCTION DEFINITIONS

===========================================================================*/


/*===========================================================================
  FUNCTION QMI_PBM_GET_RECORD_BY_REC_ID()

  DESCRIPTION
    Reads a record from PBM into a QMI data structure using PBM's record
    ID (normally this is calculated from the unique_id).

  PARAMETERS
    rec_id   : record ID to look up
    qmi_data : pointer to preallocated call history struct which will be
               populated with the newly read data

  RETURN VALUE
    QMI_ERR_NONE_V01 on success, QMI_ERR_INTERNAL_V01 on read failure, 
    QMI_ERR_NO_ENTRY_V01 if not found, QMI_ERR_INVALID_INDEX_V01 if rec_id is 
    higher than the number of records

  DEPENDENCIES
    None

  SIDE EFFECTS
    None  
===========================================================================*/
static qmi_error_type_v01 qmi_pbm_get_record_by_rec_id
(
  uint16                   rec_id,
  qmi_pbm_callhist_s_type *qmi_data
)
{
  uint16          pb_cat;
  pbm_return_type pbm_ret;
  int             num_fields;
  uint16          field_len;
  uint8           pbm_data_buf[QMI_PBM_RECORD_MAX_EFS_LEN];

  ASSERT(qmi_data);

  pbm_ret = pbm_record_read(rec_id, &pb_cat, &num_fields, pbm_data_buf,
                            QMI_PBM_RECORD_MAX_LEN);
  if (PBM_SUCCESS != pbm_ret)
  {
    LOG_MSG_ERROR_2("PBM read of ID %d failed (pbm_ret = %d)",
                    rec_id, pbm_ret);
    switch (pbm_ret)
    {
      case PBM_ERROR_NOT_FOUND:
        return QMI_ERR_NO_ENTRY_V01;
      case PBM_ERROR_INDEX_INVALID:
        return QMI_ERR_INVALID_INDEX_V01;
      default:
        return QMI_ERR_INTERNAL_V01;
    }
  }
  else if ((QMI_PBM_NUM_PB_FIELDS != num_fields) || (PBM_FIELD_AOC != ((pbm_field_s_type *)&pbm_data_buf)[0].field_id))
  {
    LOG_MSG_INFO1_2("PBM read failed: unexpected number of fields (got %d, expected %d)"
        "or a record with other field id is read", num_fields, QMI_PBM_NUM_PB_FIELDS);
    return QMI_ERR_UNKNOWN_V01;
  }

  /* If this happens, either EFS is corrupted or we fetched an entry that we 
   * didn't insert */
  field_len = qmi_pbm_get_field_len(pbm_data_buf, QMI_PBM_HIST_ENTRY_FIELD_INDEX);
  if (field_len < QMI_PBM_HIST_ENTRY_MIN_LEN || field_len > QMI_PBM_HIST_ENTRY_MAX_LEN)
  {
    LOG_MSG_ERROR_3("PBM read failed: unexpected field length (got %d, expected between %d and %d)",
                    field_len,
                    QMI_PBM_HIST_ENTRY_MIN_LEN,
                    QMI_PBM_HIST_ENTRY_MAX_LEN);
    return QMI_ERR_INTERNAL_V01;
  }

  return copy_pbm_to_qmi(pbm_data_buf, qmi_data);
}

/*===========================================================================
  FUNCTION COPY_QMI_TO_PBM()

  DESCRIPTION
    Copies data from the QMI struct into the PBM structure, also takes care
    of initializing PBM header fields and assigning the entry a unique
    identifier.

  PARAMETERS
    qmi_data      : source QMI data to copy
    pbm_data_buf  : destination buffer for PBM formatted data; should be at
                    least QMI_PB_RECORD_LEN bytes in size

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None  
===========================================================================*/
static void copy_qmi_to_pbm
(
  const qmi_pbm_callhist_s_type *qmi_data,
  uint8   *pbm_data_buf,
  uint16  *new_id,
  uint16  *data_buf_len
)
{
  qmi_pbm_callhist_s_type *data_typed;
  pbm_field_s_type        *field_typed;

  /* Initialize header values (must be before qmi_pbm_get_field_data_ptr) */
  field_typed = (pbm_field_s_type *) pbm_data_buf;

  field_typed->buffer_offset = QMI_PBM_HEADER_EFS_LEN;
  field_typed->data_len      = QMI_PBM_HIST_ENTRY_MIN_LEN;
  field_typed->field_id      = PBM_FIELD_AOC;
  field_typed->field_type    = PBM_FT_BINARY;

  /* Populate PBM buffer (AOC/binary field) with data */
  data_typed = (qmi_pbm_callhist_s_type *) 
                    qmi_pbm_get_field_data_ptr(pbm_data_buf,
                                               QMI_PBM_HIST_ENTRY_FIELD_INDEX);

  memscpy(data_typed, QMI_PBM_HIST_ENTRY_MAX_LEN, 
          qmi_data, QMI_PBM_HIST_ENTRY_MAX_LEN);

  data_typed->unique_id = qmi_pbm_get_next_unique_id();
  if (NULL != new_id)
  {
    *new_id = data_typed->unique_id;
  }

  /* Ensure phone number length is within allowable range */
  if (data_typed->phone_num_len > QMI_PBM_PHONENUM_MAX_LEN)
  {
    data_typed->phone_num_len = QMI_PBM_PHONENUM_MAX_LEN;
  }
  /* Encode the phone number using 4-bit DTMF to conserve EFS space */
  field_typed->data_len += qmi_pbm_conv_ascii_to_dtmf(data_typed->phone_num, 
                                                      data_typed->phone_num_len);

  *data_buf_len = QMI_PBM_HEADER_EFS_LEN + ALIGNED_SIZE(field_typed->data_len);
}

/*===========================================================================
  FUNCTION COPY_PBM_TO_QMI()

  DESCRIPTION
    Copies data from the structure stored in PBM into the struct used by
    QMI (essentially removes the PBM header information and handles the
    variable length phone number field).

  PARAMETERS
    pbm_data_buf  : source PBM data to copy
    qmi_data      : destination for QMI data

  RETURN VALUE
    QMI_ERR_NONE_V01 on success, QMI_ERR_INTERNAL_V01 on corrupted data_typed ptr 

  DEPENDENCIES
    None

  SIDE EFFECTS
    None  
===========================================================================*/
static qmi_error_type_v01 copy_pbm_to_qmi
(
  const uint8 *pbm_data_buf,
  qmi_pbm_callhist_s_type *qmi_data
)
{
  qmi_pbm_callhist_s_type *data_typed;

  /* First, all the data from the AOC/binary field */
  data_typed = (qmi_pbm_callhist_s_type *) 
                  qmi_pbm_get_field_data_ptr(pbm_data_buf,
                                             QMI_PBM_HIST_ENTRY_FIELD_INDEX);

  /* Check that data_typed ptr starts after the header and there are at least
     QMI_PBM_HIST_ENTRY_MIN_LEN bytes to copy*/
  /* To check for pointer arithmatic doesn't overflow 4 bytes*/
  if((MAX_UINT32 - QMI_PBM_HEADER_EFS_LEN) < (uint32)pbm_data_buf)
  {
    LOG_MSG_ERROR_0(" Invalid pbm_data_buf ptr is passed. ptr+QMI_PBM_HEADER_EFS_LEN overflowed ");
	return QMI_ERR_INTERNAL_V01;
  }
  if((MAX_UINT32 - (QMI_PBM_RECORD_MAX_EFS_LEN - QMI_PBM_HIST_ENTRY_MIN_LEN)) < (uint32)pbm_data_buf)
  {
    LOG_MSG_ERROR_0(" Invalid pbm_data_buf ptr is passed. ptr + (QMI_PBM_RECORD_MAX_EFS_LEN - QMI_PBM_HIST_ENTRY_MIN_LEN) \
	                 overflowed ");
	return QMI_ERR_INTERNAL_V01;
  }
  
  if (((uint32) data_typed <  ((uint32) pbm_data_buf + QMI_PBM_HEADER_EFS_LEN)) ||
      ((uint32) data_typed > ((uint32) pbm_data_buf + QMI_PBM_RECORD_MAX_EFS_LEN -
                             QMI_PBM_HIST_ENTRY_MIN_LEN)))
  {
    LOG_MSG_ERROR_0("data_typed ptr incorrect ");
    return QMI_ERR_INTERNAL_V01;
  }

  /* MIN_LEN will copy everything up to phone_num */
  memscpy(qmi_data, QMI_PBM_HIST_ENTRY_MIN_LEN,
          data_typed, QMI_PBM_HIST_ENTRY_MIN_LEN);

  if (qmi_data->phone_num_len > 0)
  {
    /* Make sure we aren't passing out any out of range values */
    if (qmi_data->phone_num_len > QMI_PBM_PHONENUM_MAX_LEN)
    {
      qmi_data->phone_num_len = QMI_PBM_PHONENUM_MAX_LEN;
    }
    /* To check for pointer arithmatic doesn't overflow 4 bytes*/
    if((MAX_UINT32 - (QMI_PBM_HIST_ENTRY_MIN_LEN + qmi_data->phone_num_len)) < (uint32)data_typed)
    {
      LOG_MSG_ERROR_1(" data_typed + (QMI_PBM_HIST_ENTRY_MIN_LEN + qmi_data->phone_num_len) overflowed \
      qmi_data->phone_num_len = %d ",qmi_data->phone_num_len);
      return QMI_ERR_INTERNAL_V01;
    }
    if((MAX_UINT32 - QMI_PBM_RECORD_MAX_EFS_LEN) < (uint32)pbm_data_buf)
    {
      LOG_MSG_ERROR_0(" invalid pbm_data_buf pbm_data_buf + QMI_PBM_RECORD_MAX_EFS_LEN overflowed");
      return QMI_ERR_INTERNAL_V01;
    }
	
    /* Make sure we are not reading past allocated pbm_data_buf */
    if (((uint32)data_typed + QMI_PBM_HIST_ENTRY_MIN_LEN + qmi_data->phone_num_len) >
        ((uint32)pbm_data_buf + QMI_PBM_RECORD_MAX_EFS_LEN))
    {
      LOG_MSG_ERROR_0("data_typed ptr or phone_num_len incorrect ");
      return QMI_ERR_INTERNAL_V01;
    }
    memscpy(qmi_data->phone_num, 
            qmi_data->phone_num_len,
           data_typed->phone_num,
           qmi_data->phone_num_len);
    qmi_pbm_conv_dtmf_to_ascii(qmi_data->phone_num, qmi_data->phone_num_len);
  }
  else
  {
    memset(qmi_data->phone_num, 0, QMI_PBM_PHONENUM_MAX_LEN);
  }
  return QMI_ERR_NONE_V01;
}

/*===========================================================================
  FUNCTION QMI_PBM_WRITE_RECORD()

  DESCRIPTION
    This function writes data to a record in the data service call history via
    PBM.  The fields in pbm_data_buf should already be populated, i.e.
    through copy_qmi_to_pbm().

  PARAMETERS
    pbm_data_buf :  populated PBM structure to be written
  
  RETURN VALUE
    QMI_ERR_NONE_V01 on success, QMI_ERR_INTERNAL_V01 on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None  
===========================================================================*/
static qmi_error_type_v01 qmi_pbm_write_record
(
  uint8   *pbm_data_buf,
  uint16   data_buf_len
)
{
  pbm_return_type   pbm_ret;
  uint16            rec_id;
  int               num_fields;

  if (data_buf_len > QMI_PBM_RECORD_MAX_LEN)
  {
    data_buf_len = QMI_PBM_RECORD_MAX_LEN;
  }

  rec_id = PBM_GET_FIRST_AVAILABLE;
  num_fields = QMI_PBM_NUM_PB_FIELDS;
  pbm_ret = pbm_record_validate(QMI_PBM_PB_ID, 
                                rec_id, 
                                QMI_PBM_PB_CAT, 
                                pbm_data_buf, 
                                data_buf_len, 
                                num_fields);
  if (PBM_SUCCESS != pbm_ret)
  {
    LOG_MSG_ERROR_1("QMI PBM write failed: validate failure (PBM err %d)",
                    pbm_ret);
    return QMI_ERR_INTERNAL_V01;
  }

  pbm_ret = pbm_record_write(QMI_PBM_PB_ID, 
                             &rec_id, 
                             QMI_PBM_PB_CAT, 
                             num_fields, 
                             pbm_data_buf, 
                             data_buf_len, 
                             NULL,
                             NULL); /* no CB func used - synchronous write */
  if (PBM_SUCCESS != pbm_ret)
  {
    LOG_MSG_ERROR_1("Write of PBM record failed during pbm_record_write (PBM err %d)",
                    pbm_ret);
    return QMI_ERR_INTERNAL_V01;
  }

  LOG_MSG_INFO2_1("Added new DS call history record to PBM with ID %d",
                  rec_id);
  return QMI_ERR_NONE_V01;
}

/*===========================================================================
  FUNCTION QMI_PBM_GET_FIELD_DATA_PTR()
  
  DESCRIPTION
    This function is just a shortcut return the pointer to the data buffer for
    a given field number.
  
  PARAMETERS
    field_buf : PBM record data structure
    field_num : field index in field_buf to get a data ptr for
  
  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None  
===========================================================================*/
static void * qmi_pbm_get_field_data_ptr
( 
  const void *field_buf,
  int field_num
)
{
  uint8 *ptr;
  pbm_field_s_type *ptyped_field_buf = (pbm_field_s_type *) field_buf;

  /* buffer_offset is relative to the start of this specific PBM header */
  ptr = (uint8 *) &(ptyped_field_buf[field_num]) 
                  + ptyped_field_buf[field_num].buffer_offset;
  
  return (void *) ptr;
}

/*===========================================================================
  FUNCTION QMI_PBM_GET_FIELD_LEN()
  
  DESCRIPTION
    This function is just a shortcut return the length of the data stored
    in a PBM field.
  
  PARAMETERS
    field_buf : PBM record data structure
    field_num : field index in field_buf to get a data ptr for
  
  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None  
===========================================================================*/
static uint16 qmi_pbm_get_field_len
( 
  const void *field_buf,
  int field_num
)
{
  pbm_field_s_type *ptyped_field_buf = (pbm_field_s_type *) field_buf;
  return ptyped_field_buf[field_num].data_len;
}

/*===========================================================================
  FUNCTION QMI_PBM_GET_NEXT_ID()
  
  DESCRIPTION
    Returns the next unique identifier 
  
  PARAMETERS
    None
  
  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None  
===========================================================================*/
static uint16 qmi_pbm_get_next_unique_id
( 
  void
)
{
  /* Do we need to determine where the starting point is? */
  if (QMI_PBM_UNIQUE_ID_UNINITIALIZED == qmi_pbm_unique_id)
  {
    qmi_pbm_init_unique_id();
  }

  /* Allow wraparound, but skip the special uninitialized value */
  qmi_pbm_unique_id++;
  if (QMI_PBM_UNIQUE_ID_UNINITIALIZED == qmi_pbm_unique_id)
  {
    qmi_pbm_unique_id++;
  }
  return qmi_pbm_unique_id;
}

/*===========================================================================
  FUNCTION QMI_PBM_INIT_UNIQUE_ID()
  
  DESCRIPTION
    Sets qmi_pbm_unique_id to the highest value of unique_id in our stored 
    PBM records.
  
  PARAMETERS
    None
  
  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None  
===========================================================================*/
static void qmi_pbm_init_unique_id
( 
  void
)
{
  qmi_pbm_callhist_s_type   newest_record;
  uint16                    num_recs;

  /* The list is in order of newest to oldest, so just get the newest */
  num_recs = qmi_pbm_get_list(&newest_record, NULL, 1);
  if (!num_recs)
  {
    qmi_pbm_unique_id = QMI_PBM_UNIQUE_ID_UNINITIALIZED;
  }
  else
  {
    qmi_pbm_unique_id = newest_record.unique_id;
  }
  LOG_MSG_INFO2_1("Initialized QMI PBM unique ID to %d", qmi_pbm_unique_id);
}

/*===========================================================================
  FUNCTION QMI_PBM_CONV_ASCII_TO_DTMF()
  
  DESCRIPTION
    Converts a dial string in-place from 8-bit ASCII to 4-bit DTMF (based
    on 3GPP 24.008)
  
  PARAMETERS
    target : current location of ASCII digits, will be filled with DTMF data
    len    : length of phone number
  
  RETURN VALUE
    The length of the DTMF encoded data

  DEPENDENCIES
    None

  SIDE EFFECTS
    None  
===========================================================================*/
static uint8 qmi_pbm_conv_ascii_to_dtmf
(
  char            *target,
  uint8           len
)
{
  uint8   ascii_backup[QMI_PBM_PHONENUM_MAX_LEN];
  uint8   ascii_index;
  uint8   dtmf_index;
  uint8   max_len;

  max_len = MIN(len, QMI_PBM_PHONENUM_MAX_LEN);

  memscpy(ascii_backup, max_len, target, max_len);

  for (dtmf_index = ascii_index = 0; ascii_index < max_len; ascii_index++)
  {
    target[dtmf_index] = qmi_pbm_ascii_char_to_dtmf(ascii_backup[ascii_index]);
    if (++ascii_index < max_len)
    {
      target[dtmf_index] |= (qmi_pbm_ascii_char_to_dtmf(ascii_backup[ascii_index]) << 4) & 0xF0;
    }
    dtmf_index++;
  }
  return dtmf_index;
}

/*===========================================================================
  FUNCTION QMI_PBM_CONV_DTMF_TO_ASCII()
  
  DESCRIPTION
    Converts a dial string in-place from 4-bit DTMF to 8-bit ASCII (based
    on 3GPP 24.008)
  
  PARAMETERS
    target : current location of ASCII digits, will be filled with DTMF data
    len    : length of phone number
  
  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None  
===========================================================================*/
static void qmi_pbm_conv_dtmf_to_ascii
(
  char            *target,
  uint8           len
)
{
  uint8   dtmf_backup[QMI_PBM_PHONENUM_MAX_LEN];
  uint8   ascii_index;
  uint8   dtmf_index;
  uint8   dtmf_val;

  memscpy(dtmf_backup, QMI_PBM_PHONENUM_MAX_LEN, 
          target, MIN(len, QMI_PBM_PHONENUM_MAX_LEN));

  for (dtmf_index = ascii_index = 0; ascii_index < len; dtmf_index++)
  {
    dtmf_val = dtmf_backup[dtmf_index] & 0x0F;
    target[ascii_index++] = qmi_pbm_dtmf_to_ascii_lut[dtmf_val];
    if (ascii_index < len)
    {
      dtmf_val = (dtmf_backup[dtmf_index] >> 4) & 0x0F;
      target[ascii_index++] = qmi_pbm_dtmf_to_ascii_lut[dtmf_val];
    }
  }
}

/*===========================================================================
  FUNCTION QMI_PBM_ASCII_CHAR_TO_DTMF()
  
  DESCRIPTION
    Input an ASCII value, outputs a DTMF 4-bit encoded value.
  
  PARAMETERS
    ascii_val : input ASCII value (valid chars: 0-9, *, #)
  
  RETURN VALUE
    4-bit DTMF encoded value

  DEPENDENCIES
    None

  SIDE EFFECTS
    None  
===========================================================================*/
static inline uint8 qmi_pbm_ascii_char_to_dtmf
(
  char ascii_val
)
{
  if (ascii_val >= '1' && ascii_val <= '9')
  {
    return (ascii_val - '0');
  }
  else
  {
    switch(ascii_val)
    {
      case '0':
        return QMI_PBM_DTMF_ZERO;
      case '*':
        return QMI_PBM_DTMF_STAR;
      case '#':
        return QMI_PBM_DTMF_POUND;
      case '\0':
        return QMI_PBM_DTMF_NULL;
    }
  }
  LOG_MSG_INFO1_1("WARNING: Unable to convert ASCII val 0x%x to DTMF (using '?' in place)",
                  ascii_val);
  return QMI_PBM_DTMF_UNKNOWN;
}

