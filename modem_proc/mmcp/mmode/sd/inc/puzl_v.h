#ifndef PUZL_V_H
#define PUZL_V_H

/*===========================================================================

                   P R E F E R R E D   U S E R   Z O N E   L I S T

                             H E A D E R   F I L E

DESCRIPTION
  This header file contains all the declarations necessary for
  PUZL module interface

Copyright (c) 2001 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/sd/inc/puzl_v.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/29/01   SJ      Initial release.

===========================================================================*/


#include "customer.h"  /* Customer configuration file */
#include "comdef.h"    /* Definition for basic types and macros */
#include "nv.h"        /* Interface to NV */
#include "puzl.h"




/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
============================== EXTERNAL TYPES ===============================
=============================================================================
=============================================================================
===========================================================================*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Defines for field lengths in PUZL as given in I-683B specifications
** useful when extracting bit packed fields in PUZL.
*/

#define PUZLI_RESERVED_1_LEN           2
#define PUZLI_UZ_TEMP_SUBSC_LEN        1
#define PUZLI_UZ_ORIG_ONLY_LEN         1
#define PUZLI_NUM_UZ_ENTRIES_LEN       12
#define PUZLI_UZ_PRIORITY_LEN          4
#define PUZLI_UZ_ID_LEN                16
#define PUZLI_UZ_SID_LEN               15
#define PUZLI_UZ_NAME_ENCODING_LEN     8
#define PUZLI_N_OCTETS_LEN             5
#define PUZLI_UZ_NAME_OCTET_LEN        8
#define PUZLI_ACTIVE_FLAG_LEN          1
#define PUZLI_PRIORITY_CONTROL_LEN     3
#define PUZLI_NOTIFICATION_FLAG_LEN    1
#define PUZLI_UZ_REVISION_LEN          4
#define PUZLI_UZ_TYPE_LEN              3
#define PUZLI_NUM_ELEMENTS_LEN         4
#define PUZLI_OHP_TYPE_LEN             3
#define PUZLI_N_ITEMS_LEN              5
#define PUZLI_NID_LEN                  16
#define PUZLI_HYSTERESIS_LEN           4

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Max number of "Sid and Nid" ohp type.
*/
#define  PUZL_MAX_N_ITEMS              32

/* Max number of User Zone type specific fields.
*/
#define  PUZL_MAX_NUM_ELEMENTS         16


/* PUZL define for invalid entry index.
*/
#define  PUZL_INVLD_UZ_ENTRY_INDEX     (-1)

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Typedef for the User zone priority.
*/
typedef byte puzl_uz_pri_t_type;

/* typedef for user zone revision.
*/
typedef byte puzl_uz_rev_t_type;

/* typedef for User zone ID.
*/
typedef int4  puzl_uz_id_t_type;

/* typedef for bit offset in puzl table
** Maximum is 64K, so PUZL table should be less than 8K bytes.
*/
typedef  word  puzl_offset_t_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Typedef for the CB function for nv read function.
*/
typedef boolean (puzl_nv_read_f_type) (
    nv_items_enum_type        nv_item,
       /* NV item to read.
       */

    nv_item_type              *data_ptr
       /* Pointer to a data buffer where to store the data that is
       ** associated with the NV item.
       */
);

/* Typedef for the CB function for PUZL read function
*/
typedef boolean (puzl_read_f_type) (
    byte                     nam,
       /* NAM for which PUZL is to be read
       */

    byte                     *data_ptr,
      /* Pointer to a data buffer where to store the data that is
      ** associated with the PUZL
      */

    int32                    *size_ptr
      /* On input this parameter is the  maximum number
      ** of bytes to be copied into the buffer pointed by data_ptr.
      ** This parameter must not be nil.
      ** On output it is set *size_ptr to the actual number of bytes
      ** that were copied into the buffer pointed by data_ptr.
      */
);

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Enumeration for MS specific OHP determined - user zone types
** I-683B specifications  3GPP2 C.S0016-A, Section 3.5.7.1.3.
*/
typedef enum {
  PUZL_UZ_OHP_SID_ONLY             = 0,
  PUZL_UZ_OHP_SID_AND_NID          = 1,  /* Supported OHP type */
  PUZL_UZ_OHP_SID_AND_BASE_ID      = 2,
  PUZL_UZ_OHP_BASE_ST_LAT_AND_LONG = 3,
  PUZL_UZ_OHP_UZ_CENTER_PT_AND_RAD = 4,
  PUZL_UZ_OHP_MAX
} puzl_uz_ohp_e_type;

/* Enumeration for temporary subscription for the MS. This indicates whether
** or not the MS is permitted to access user zones that offer temporary
** subscription.
** I-683B specifications  3GPP2 C.S0016-A, Section 3.5.7.
*/
typedef enum {
  PUZL_UZ_TEMP_SUBSC_DISALLOWED       = 0,
  PUZL_UZ_TEMP_SUBSC_ALLOWED          = 1,
  PUZL_UZ_TEMP_SUBSC_MAX
} puzl_uz_temp_subsc_e_type;

/* Enumeration for user zone origination type, this indicates whether or not
** the MS is permitted to originate calls only when within the serving area of
** currently assigned user zone.
** I-683B specifications 3GPP2 C.S0016-A, Section 3.5.7
*/
typedef enum {
  PUZL_UZ_ORIG_ONLY_ANYWHERE       = 0,
  PUZL_UZ_ORIG_ONLY_WITHIN_UZ      = 1,
  PUZL_UZ_ORIG_ONLY_MAX
} puzl_uz_orig_only_e_type;

/* Enumeration for User zone Notification flag type.
** I-683B specifications 3GPP2 C.S0016-A, Section 3.5.7.
*/
typedef enum {
  PUZL_UZ_NOTIF_FLAG_FALSE         = 0,
  PUZL_UZ_NOTIF_FLAG_TRUE          = 1,
  PUZL_UZ_NOTIF_FLAG_MAX
} puzl_uz_notif_flag_e_type;

/* Enumeration for User zone priority control type
** I-683B specifications 3GPP2 C.S0016-A, Section 3.5.7.
*/
typedef enum {
  PUZL_UZ_PRI_CTRL_NO_CHANGE        = 0,
  PUZL_UZ_PRI_CTRL_MANUAL           = 1,
  PUZL_UZ_PRI_CTRL_MAN_TEMP         = 2,
  PUZL_UZ_PRI_CTRL_MAX
} puzl_uz_pri_cntrl_e_type;

/* Enumeration for User Zone types
** I-683B specifications - 3GPP2 C.S0016-A, Section 3.5.7, Table 3.5.7-1
*/
typedef enum {
  PUZL_UZ_BRDCST_RF_COV_BASED        = 0,
  PUZL_UZ_BRDCST_GEO_BASED           = 1,
  PUZL_UZ_MS_SP_RF_COV_BASED_OHP_DET = 2,  /* Supported type */
  PUZL_UZ_MS_SP_GEO_BASED            = 3,
  PUZL_UZ_MS_SP_GEO_BASED_SEP_CAR    = 4,
  PUZL_UZ_MAX
} puzl_uz_e_type;

/* Enumeration for PUZL initialization return type.
*/
typedef enum {
  PUZL_ERROR_NOT_INITIALIZED,      /* deliberately made this 0, since static
                                   ** will init to 0
                                   */
  PUZL_ERROR_NO_ERR,               /* PUZL module initialized with no
                                   ** errors
                                   */
  PUZL_ERROR_INV_NUM_OF_RECORDS,   /* PUZL table contains invalid num of
                                   ** records
                                   */
  PUZL_ERROR_UNSUPPORTED_UZ_TYPE,  /* PUZL table has user zone record with
                                   ** unsupported user zone type
                                   */
  PUZL_ERROR_INVALID_UZ_RECORD,    /* PUZL table has invalid ohp types,
                                   ** n_items = 0
                                   */
  PUZL_ERROR_NOT_ENABLED,          /* PUZL is not enabled in NV */
  PUZL_ERROR_NVRAM_READ_FAILED,    /* PUZL table nv ram read failed */
  PUZL_ERROR_MAX
} puzl_error_e_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Internal data structure for Overhead Parameter Determined - SID and NID,
** User Zone entry type.
*/
typedef struct {
  word     num_uz_ohp_items;
  struct {
    word nid;
    byte hysteresis;
  } uz_ohp_sid_nid_entry[PUZL_MAX_N_ITEMS];
} puzl_uz_ohp_sid_nid_s_type;

/* Internal Data structure for Mobile Specific RF Coverage Based - Overhead
** Parameter Determined User Zone type.
*/
typedef struct {
  /* Only PUZL_OHP_SID_AND_NID supported.
  */
  puzl_uz_ohp_e_type   uz_ohp_type;
  union {
    puzl_uz_ohp_sid_nid_s_type ohp_sid_nid_items;
    /* Other ohp types if needed should be added here.
    */
  } uz_ohp_type_fields;
} puzl_uz_rf_cov_ohp_s_type;


/* Type to hold the user zone type specific fields.
*/
typedef union {
  puzl_uz_rf_cov_ohp_s_type   uz_ohp;
} puzl_uz_type_sp_fields_s_type;

/* Extracted User Zone record type, no validations performed on unused
** fields.
*/
typedef struct {
  puzl_uz_temp_subsc_e_type       uz_temp_subsc;     /* UNUSED FIELD */
  puzl_uz_orig_only_e_type        uz_orig_only;      /* UNUSED FIELD */
  puzl_uz_pri_t_type              uz_priority;       /* UNUSED FIELD */
  puzl_uz_id_t_type               uz_id;
  word                            uz_sid;
  puzl_uz_name_s_type             uz_name;
  puzl_uz_act_flag_e_type         active_flag;
  puzl_uz_pri_cntrl_e_type        priority_control;  /* UNUSED FIELD */
  puzl_uz_notif_flag_e_type       notification_flag; /* UNUSED FIELD */
  puzl_uz_rev_t_type              uz_revision;       /* UNUSED FIELD */
  puzl_uz_e_type                  uz_type;
  word                            num_uz_type_elem;
  puzl_uz_type_sp_fields_s_type   uz_type_info[PUZL_MAX_NUM_ELEMENTS];
} puzl_uz_record_s_type;



/* ^L<EJECT> */
/*===========================================================================

FUNCTION
  puzl_init

DESCRIPTION
   This is the first function to be called on PUZL module, before any other
  can be called. This function reads the PUZL from NVRAM and validates it.

DEPENDENCIES
  None.

RETURN VALUE
  return indicates whether the PUZL module initialization was successful
  or the cause for failure.

SIDE EFFECTS
  None.

===========================================================================*/
extern  puzl_error_e_type             puzl_init(

        byte                          nam,
          /*  NAM with which to initialize the PUZL module.
          */

       puzl_nv_read_f_type           *nv_read_cb_func,
         /* Pointer to a NV Read callback function.
         */

       puzl_read_f_type              *puzl_read_cb_func
         /* Pointer to a NV PUZL read callback function.
         */
);

/* ^L<EJECT> */
/*===========================================================================

FUNCTION
  puzl_get_init_err_code

DESCRIPTION
  This function returns the initialization error code

DEPENDENCIES
  None.

RETURN VALUE
  PUZL_ERROR_NO_ERR => PUZL successfully initialized
  !PUZL_ERROR_NO_ERR => PUZL init failed

SIDE EFFECTS
  None.

===========================================================================*/
extern  puzl_error_e_type             puzl_get_init_err_code(
        void
);

/* ^L<EJECT> */
/*===========================================================================

FUNCTION
  puzl_find_record_with_sidnid

DESCRIPTION
  This extern function finds matching user zone record given the sid and nid.

DEPENDENCIES
  None.

RETURN VALUE
  PUZL_INVLD_UZ_ENTRY_INDEX if no match found,
  Any other value - match was found.

SIDE EFFECTS
  None.

===========================================================================*/
extern  int4                  puzl_find_record_with_sidnid(

        word                  sid,
        /*  Sid for which user zone record is to be searched in PUZL.
        */

        word                  nid
        /*  Nid for which user zone record is to be searched in PUZL.
        */
);

/* ^L<EJECT> */
/*===========================================================================

FUNCTION
  puzl_extract_uz_record

DESCRIPTION
  This extern function extracts the user zone record given the entry index.

DEPENDENCIES
  None.

RETURN VALUE
  !NULL - extract was successful
  == NULL - extract failed

SIDE EFFECTS
  None.
===========================================================================*/
extern  puzl_uz_record_s_type*               puzl_extract_uz_record(

        int4                                 index
        /*  user zone entry index in the PUZL for which record
        **  has to be extracted.
        */
);

#endif /* PUZL_V_H */
