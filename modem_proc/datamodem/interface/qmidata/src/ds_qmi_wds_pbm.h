#ifndef DS_QMI_WDS_PBM_H
#define DS_QMI_WDS_PBM_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                 D S _ Q M I _ W D S _ P B M . H
DESCRIPTION
  The Data Services QMI - Phone Book Manager (PBM) header file.  Used by
  QMI to perform data service call history operations.
                                               
Copyright (c) 2008 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/qmidata/src/ds_qmi_wds_pbm.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $ 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/21/08   bd      Created module
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "customer.h"

#include "cm.h"
#include "ds_qmi_defs.h"
#include "pbm_tuning.h"
#include "wireless_data_service_v01.h"


/*===========================================================================

                      DATA DEFINITIONS AND DECLARATIONS 

===========================================================================*/

#define QMI_PBM_MAX_PB_ENTRIES      PBM_MAX_CALL_HISTORY
#define QMI_PBM_PHONENUM_MAX_LEN    CM_MAX_NUMBER_CHARS
#define QMI_PBM_HIST_ENTRY_MAX_LEN     (sizeof(qmi_pbm_callhist_s_type))
#define QMI_PBM_HIST_ENTRY_MIN_LEN     (sizeof(qmi_pbm_callhist_s_type) - QMI_PBM_PHONENUM_MAX_LEN)

/* Currently, a record only has 1 PBM field ("complex") */
#define QMI_PBM_RECORD_MAX_LEN      QMI_PBM_HIST_ENTRY_MAX_LEN
#define QMI_PBM_RECORD_MIN_LEN      QMI_PBM_HIST_ENTRY_MIN_LEN

/* Maximum list length includes an extra 2 bytes for the number of entries included */
#define QMI_PBM_LIST_MAX_LEN        (2 + QMI_PBM_MAX_PB_ENTRIES * QMI_PBM_RECORD_MAX_LEN)

/* Masks for qmi_pbm_get_var_len_list() */
#define QMI_PBM_MASK_UNIQUE_ID        0x0001
#define QMI_PBM_MASK_SOURCE           0x0002
#define QMI_PBM_MASK_TECH_TYPE        0x0004
#define QMI_PBM_MASK_DATE             0x0008
#define QMI_PBM_MASK_IPADDR           0x0010
#define QMI_PBM_MASK_DURATION         0x0020
#define QMI_PBM_MASK_DURATION_ACTIVE  0x0040
#define QMI_PBM_MASK_RX_BYTES         0x0080
#define QMI_PBM_MASK_TX_BYTES         0x0100
#define QMI_PBM_MASK_CALL_ERR         0x0200
#define QMI_PBM_MASK_PHONE            0x0400 /* includes both phone_num_len and phone_num */

/* All data fields included */
#define QMI_PBM_MASK_ALL              0x07FF

#define QMI_PBM_SOURCE_RMNET  0
#define QMI_PBM_SOURCE_DUN    1

/* The struct we use to store all our data in PBM (note that this is not
 * quite the same as the structure used to send data from QMI to client - 
 * that has a variable-length phone_num field while this one is fixed) */
typedef PACKED struct PACKED_POST
{
  /* NOTE: the unique_id field is valid only when passing data
   * out of PBM; i.e. you can't specify the ID of a new entry */
  uint16  unique_id;
  uint8   source; 
  uint8   tech_type_e;
  uint64  date;
  uint32  ipv4_addr;
  uint64  call_duration;
  uint64  call_duration_active;
  uint64  rx_bytes;
  uint64  tx_bytes;
  uint16  call_err_e;

  /* NOTE: the phone_num_len field is only populated when passing data out
   * of PBM (i.e. from a read) - it is ignored when making a write to PBM */
  uint8   phone_num_len;
  /* This field must always be the last member of the struct */
  char    phone_num[QMI_PBM_PHONENUM_MAX_LEN];
} qmi_pbm_callhist_s_type;


/*===========================================================================

                        EXTERNAL FUNCTION DEFINITIONS 
 
===========================================================================*/


/*===========================================================================
  FUNCTION QMI_PBM_ADD_RECORD()

  DESCRIPTION
    Creates and writes a new PBM data service call history entry.  If the 
    write is successful, rec_id will contain the new PBM record ID.

  PARAMETERS
    qmi_data : contains all data to write into PBM
    rec_id   : (out) record ID of the newly inserted entry

  RETURN VALUE
    QMI_ERR_NONE_V01 on success, QMI_ERR_INTERNAL_V01 on write failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None  
===========================================================================*/
qmi_error_type_v01 qmi_pbm_add_record
(
  const qmi_pbm_callhist_s_type *qmi_data,
  uint16 *rec_id
);

/*===========================================================================
  FUNCTION QMI_PBM_GET_NUM_RECORDS()

  DESCRIPTION
    Returns the number of data service call history entries currently stored
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
uint16 qmi_pbm_get_num_records( void );

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
);

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
  wds_call_history_list_resp_msg_v01 *resp_msg,
  uint16  resp_size,
  uint16  field_mask
);

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
);

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
qmi_error_type_v01 qmi_pbm_clear_history( void );

#endif /* DS_QMI_WDS_PBM_H */

