/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====
 *
 *                 MCFG Diagnostics Packet Processing
 *
 *
 *  Copyright (C) 2002-2014  QUALCOMM Technologies, Incorporated.
 *  All Rights Reserved.
 *
 *  Diagnostic packet processing routines for MCFG
 *
 *
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


 /*===========================================================================

                           Edit History

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcfg_fwk/mcfg_diag/src/mcfg_diag.c#1 $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/16/15   sbt      Initial Creation
===========================================================================*/

#include "comdef.h"
#include "diagbuf.h"
#include "diagcmd.h"
#include "diagpkt.h"
#include "mcfg_fs.h"
#include "mcfg_int.h"
#include "mcfg_feature_config.h"
#include <stringl/stringl.h>

#ifdef FEATURE_MCFG_DIAG_SUPPORT

#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include "fs_public.h"
#include "fs_errno.h"
#include "diag.h"
#include "mcfg_diag.h"

#include "mcfg_diag_i.h"

/* -------------------------------------------------------------------------
** Definitions
** ------------------------------------------------------------------------- */

typedef struct {
  uint32 targ_pkt_window;       /* Target window size in packets            */
  uint32 targ_byte_window;      /* Target window size in bytes              */
  uint32 host_pkt_window;       /* Host window size in packets              */
  uint32 host_byte_window;      /* Host window size in bytes                */
  uint32 iter_pkt_window;       /* Dir iteration window size in packets     */
  uint32 iter_byte_window;      /* Dir iteration window size in bytes       */
  int32  version;               /* Protocol version number                  */
  int32  min_version;           /* Minimum supported protocol version       */
  int32  max_version;           /* Maximum supported protocol version       */
  int32  feature_bits;          /* Bit mask of supported features           */
} mcfg_fs_diag_params_type;


/* Lint wants parens around macro but compile will fail */
/*lint -save -e773 */
#define VOID_PACKED_PTR        PACKED void * PACKED_POST
#define MCFG_DECLARE_DIAG_FN(fn)    VOID_PACKED_PTR fn (VOID_PACKED_PTR, uint16)
/*lint -restore */

MCFG_DECLARE_DIAG_FN (mcfg_fs_diag_put_handler);
MCFG_DECLARE_DIAG_FN (mcfg_fs_diag_get_handler);
MCFG_DECLARE_DIAG_FN (mcfg_fs_diag_stat_handler);

/* -------------------------------------------------------------------------
** Local Data
** ------------------------------------------------------------------------- */
static mcfg_fs_diag_params_type mcfg_fs_diag_params;
static const mcfg_fs_diag_params_type mcfg_fs_diag_default_params = {
  MCFG_FS_TARG_PKT_WINDOW_DEFAULT,
  MCFG_FS_TARG_BYTE_WINDOW_DEFAULT,
  MCFG_FS_HOST_PKT_WINDOW_DEFAULT,
  MCFG_FS_HOST_BYTE_WINDOW_DEFAULT,
  MCFG_FS_ITER_PKT_WINDOW_DEFAULT,
  MCFG_FS_ITER_BYTE_WINDOW_DEFAULT,
  MCFG_FS_DIAG_VERSION,
  MCFG_FS_DIAG_MIN_VERSION,
  MCFG_FS_DIAG_MAX_VERSION,
  MCFG_FS_FEATURE_BITS
};


/* The dispatch table for the MCFG subsystem.
*/
static const diagpkt_user_table_entry_type mcfg_diag_tbl[] =
{
  {MCFG_FS_DIAG_PUT,     MCFG_FS_DIAG_PUT,   mcfg_fs_diag_put_handler},
  {MCFG_FS_DIAG_GET,     MCFG_FS_DIAG_GET,   mcfg_fs_diag_get_handler},
  {MCFG_FS_DIAG_STAT,    MCFG_FS_DIAG_STAT,  mcfg_fs_diag_stat_handler},
};

#define MCFG_SUBSYS DIAG_SUBSYS_MCFG

#ifdef  DIAG_MAX_RX_PKT_SIZ
  #define MCFG_LARGEST_DIAG_PACKET     DIAG_MAX_RX_PKT_SIZ
#else
  #define MCFG_LARGEST_DIAG_PACKET     0x800
#endif

/**********************************************************************
 * Packet pathname validation.
 *********************************************************************/

/* Count the number of nul characters in the given string. */
static unsigned
mcfg_fsd_count_nulls (PACKED char *  PACKED_POST name, unsigned length)
{
  unsigned i;
  unsigned result = 0;

  for (i = 0; i < length; i++) {
    if (name[i] == '\0')
      result++;
  }

  return result;
}

/* Convenience macro for checking pathnames.  Takes its argument, and
 * verifies that it contains at least as many nulls.  Arguments are:
 *   request - The typed request packet.
 *   type    - The type of the request packet.
 *   field   - The name of the field containing the pathname
 *   req_len - The total length of the packet
 *   min_nul - Miminum number of nul characters in the string.
 * This macro contains a return, and will return a diag error packet
 * indicating a bad parameter if the request doesn't contain sufficient
 * nulls. */
#define MCFG_FSD_CHECK_NULLS(_req, _type, _field, _req_len, _min_nul) \
  do { \
    if (mcfg_fsd_count_nulls ((_req)->_field, \
          (_req_len) - offsetof (_type, _field)) < (_min_nul)) \
    { \
      return diagpkt_err_rsp (DIAG_BAD_PARM_F, \
          (void *) (_req), (_req_len)); \
    } \
  } while (0)


/**********************************************************************
 * Packet length validation.
 *********************************************************************/

static int
mcfg_fs_diag_is_valid_pkt_len (uint32 pkt_len, uint32 min_pkt_len)
{
  /* Validate pkt_len range */
  if (pkt_len > MCFG_LARGEST_DIAG_PACKET || pkt_len < min_pkt_len)
  {
    return 0;
  }
  return 1;
}

/**********************************************************************
 * Convert the error code given in the target to the one published in
 * efs diag documentation
 *********************************************************************/
uint32
mcfg_fs_get_diag_errno (uint32 posix_errno)
{
  uint32 diag_errno;

  switch (posix_errno)
  {
    case EPERM:
      diag_errno = 1;
      break;
    case ENOENT:
      diag_errno = 2;
      break;
    case EEXIST:
      diag_errno = 6;
      break;
    case EBADF:
      diag_errno = 9;
      break;
    case ENOMEM:
      diag_errno = 12;
      break;
    case EACCES:
      diag_errno = 13;
      break;
    case EBUSY:
      diag_errno = 16;
      break;
    case EXDEV:
      diag_errno = 18;
      break;
    case ENODEV:
      diag_errno = 19;
      break;
    case ENOTDIR:
      diag_errno = 20;
      break;
    case EISDIR:
      diag_errno = 21;
      break;
    case EINVAL:
      diag_errno = 22;
      break;
    case EMFILE:
      diag_errno = 24;
      break;
    case ETXTBSY:
      diag_errno = 26;
      break;
    case ENOSPC:
      diag_errno = 28;
      break;
    case ESPIPE:
      diag_errno = 29;
      break;
    case FS_ERANGE:
      diag_errno = 34;
      break;
    case ENAMETOOLONG:
      diag_errno = 36;
      break;
    case ENOTEMPTY:
      diag_errno = 39;
      break;
    case ELOOP:
      diag_errno = 40;
      break;
    case ETIMEDOUT:
      diag_errno = 110;
      break;
    case ESTALE:
      diag_errno = 116;
      break;
    case EDQUOT:
      diag_errno = 122;
      break;
    case ENOCARD:
      diag_errno = 301;
      break;
    case EBADFMT:
      diag_errno = 302;
      break;
    case ENOTITM:
      diag_errno = 303;
      break;
    case EROLLBACK:
      diag_errno = 304;
      break;
    case ENOTHINGTOSYNC:
      diag_errno = 306;
      break;
    case EEOF:
      diag_errno = 0x8000;
      break;
    case EUNKNOWN_SFAT:
      diag_errno = 0x8001;
      break;
    case EUNKNOWN_HFAT:
      diag_errno = 0x8002;
      break;
    default:
      /* Do Nothing if we dont need to convert */
      diag_errno = posix_errno;
      break;
  }

  return diag_errno;
}

/*===========================================================================

FUNCTION MCFG_DIAG_INIT
DESCRIPTION
  Initialization function for this module. Registers the packet handlers
  and sets up local data structures.
============================================================================*/
void mcfg_diag_init (void)
{
  unsigned i;

  MCFG_MSG_LOW("====  Registering MCFG Diag cmds ==== ");
  mcfg_fs_diag_params = mcfg_fs_diag_default_params;

  /* Register MCFG diag packets. */
  DIAGPKT_DISPATCH_TABLE_REGISTER (MCFG_SUBSYS, mcfg_diag_tbl);

  return;
} /* END mcfg_diag_init */


/*===========================================================================

FUNCTION MCFG_FS_DIAG_STAT_HANDLER
DESCRIPTION
  Handles the MCFG_FS_DIAG_STAT packet.
============================================================================*/
VOID_PACKED_PTR
mcfg_fs_diag_stat_handler (VOID_PACKED_PTR req_ptr, uint16 pkt_len)
{
  mcfg_fs_diag_stat_req_type *request;
  mcfg_fs_diag_stat_rsp_type *response;
  uint16 rsp_len = sizeof (mcfg_fs_diag_stat_rsp_type);
  struct fs_stat sbuf;
  const char *request_path;

  /*----------------------------------------------------------------------
    Check security, since this is a secure funciton
    ----------------------------------------------------------------------*/
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED) {
    return( diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len) );
  }

  if (pkt_len < sizeof (mcfg_fs_diag_stat_req_type))
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);

  request = (mcfg_fs_diag_stat_req_type *) req_ptr;
  request_path = (const char *) request->path;

  MCFG_FSD_CHECK_NULLS (request, mcfg_fs_diag_stat_req_type,
      path, pkt_len, 1);

  response = (mcfg_fs_diag_stat_rsp_type *)
    diagpkt_subsys_alloc (MCFG_SUBSYS,
                          MCFG_FS_DIAG_STAT,
                          rsp_len);
  if (response == NULL)
    return NULL;

  MCFG_MSG_MED_2("fpath: %s, sub_id: %d", request_path, request->sub_id);

  if (mcfg_fs_stat (request_path, 
                &sbuf,
                request->fs_type,
                request->sub_id) == 0)
  {
    response->mode   = (int32) sbuf.st_mode;
    response->size   = (int32) sbuf.st_size;
    response->nlink  = (int32) sbuf.st_nlink;
    response->atime  = (int32) sbuf.st_atime;
    response->mtime  = (int32) sbuf.st_mtime;
    response->ctime  = (int32) sbuf.st_ctime;
    response->diag_errno  = 0;
  }
  else
  {
    response->mode   = 0;
    response->size   = 0;
    response->nlink  = 0;
    response->atime  = 0;
    response->mtime  = 0;
    response->ctime  = 0;
    response->diag_errno  = mcfg_fs_get_diag_errno (efs_errno);
  }

  return response;
} /* END mcfg_fs_diag_stat_handler */


/*===========================================================================

FUNCTION MCFG_FS_DIAG_GET_HANDLER
DESCRIPTION
  Handles the MCFG_FS_DIAG_GET packet. The field sequence_number is used for the
  purpose of Synchronization / Correlation purpose. The sequence_number
  received is fetched out of request packet and tucked back into the response
  packet. This would benefit the application with synchronized requests and
  responses.

============================================================================*/
VOID_PACKED_PTR
mcfg_fs_diag_get_handler (VOID_PACKED_PTR req_ptr, uint16 pkt_len)
{
  mcfg_fs_diag_get_req_type *request;
  mcfg_fs_diag_get_rsp_type *response;
  unsigned rsp_len = 0;
  unsigned base_length = 0;
  mcfg_fs_status_e_type read_status = MCFG_FS_STATUS_OK;
  const char *request_path;
  uint16 seq_num;

  /*----------------------------------------------------------------------
    Check security, since this is a secure function
    ----------------------------------------------------------------------*/
  if (diag_get_security_state () != DIAG_SEC_UNLOCKED) {
    return( diagpkt_err_rsp (DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len));
  }

  if (pkt_len < sizeof (mcfg_fs_diag_get_req_type))
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);

  request = (mcfg_fs_diag_get_req_type *) req_ptr;
  request_path = (const char *) request->path;
  /*
   * Fetch the Sequence number commonly used by the calling application
   * for Synchronization / Correlation purpose.
   */
  seq_num = request->sequence_number;

  MCFG_FSD_CHECK_NULLS (request, mcfg_fs_diag_get_req_type, path,
      pkt_len, 1);

  if (request->data_length > MCFG_FS_DIAG_MAX_READ_REQ) {
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);
  }

  base_length = FPOS (mcfg_fs_diag_get_rsp_type, data);
  rsp_len = base_length + request->data_length;

  response = (mcfg_fs_diag_get_rsp_type *)
    diagpkt_subsys_alloc (MCFG_SUBSYS,
                          MCFG_FS_DIAG_GET,
                          rsp_len);
  if (response == NULL)
    return NULL;

  /* Acknowledge back with the Sequence number */
  response->sequence_number = seq_num;

  MCFG_MSG_MED_3("fpath: %s, fsz: %d, sub_id: %d", request_path, request->data_length, request->sub_id);

  read_status = mcfg_fs_read (request_path,
                        (void *) response->data,
                        request->data_length,
                        request->fs_type,
                        request->sub_id );

  if (read_status == MCFG_FS_STATUS_OK) {
    response->diag_errno      = 0;
  } else { 
    response->diag_errno = mcfg_fs_get_diag_errno (efs_errno);
    diagpkt_shorten (response, base_length);
  }

  return response;
} /* END mcfg_fs_diag_get_handler */


/*===========================================================================

FUNCTION MCFG_FS_DIAG_PUT_HANDLER
DESCRIPTION
  Handles the MCFG_FS_DIAG_PUT packet. The field sequence_number is used for the
  purpose of Synchronization / Correlation purpose. The sequence_number
  received is fetched out of request packet and tucked back into the response
  packet. This would benefit the application with synchronized requests and
  responses.

============================================================================*/
VOID_PACKED_PTR
mcfg_fs_diag_put_handler (VOID_PACKED_PTR req_ptr, uint16 pkt_len)
{
  mcfg_fs_diag_put_req_type *request;
  mcfg_fs_diag_put_rsp_type *response;
  uint16 rsp_len = sizeof (mcfg_fs_diag_put_rsp_type);
  fs_ssize_t nbyte;
  mcfg_fs_status_e_type write_status = MCFG_FS_STATUS_OK;
  unsigned   data_offset;
  char      *put_locn;
  char      *put_path;
  uint16 seq_num;

  /*----------------------------------------------------------------------
    Check security, since this is a secure function
    ----------------------------------------------------------------------*/
  if (diag_get_security_state () != DIAG_SEC_UNLOCKED) {
    return( diagpkt_err_rsp (DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len));
  }

  if (pkt_len < sizeof (mcfg_fs_diag_put_req_type))
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);

  request = (mcfg_fs_diag_put_req_type *) req_ptr;

  put_locn = (char *) request->data;
  nbyte    = request->data_length;
  put_path = (char *) request->data + nbyte;
  data_offset = FPOS (mcfg_fs_diag_put_req_type, data);
  /*
   * Fetch the Sequence number usually used by the calling application
   * for Synchronization / Correlation purpose.
   */
  seq_num = request->sequence_number;

  /* Validate this packet, more complicated than others, because there is a
   * null-terminated string after variable length data. */
  if ((nbyte + data_offset >= pkt_len) ||
      (mcfg_fsd_count_nulls (put_path, pkt_len - (data_offset + nbyte)) < 1))
  {
    return (diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len));
  }

  response = (mcfg_fs_diag_put_rsp_type *)
    diagpkt_subsys_alloc (MCFG_SUBSYS,
                          MCFG_FS_DIAG_PUT,
                          rsp_len);
  if (response == NULL)
    return NULL;

  /* Acknowledge back with the Sequence number */
  response->sequence_number = seq_num;

  MCFG_MSG_MED_3("fpath: %s, fsz: %d, sub_id: %d", put_path, request->data_length, request->sub_id);

  write_status = mcfg_fs_write (put_path,
                           (void *) put_locn,
                           nbyte, request->flags,
                           0777, 
                           request->fs_type,
                           request->sub_id );

  if (write_status != MCFG_FS_STATUS_OK)
  {
    response->diag_errno = mcfg_fs_get_diag_errno (efs_errno);
  }
  else
  {
    response->diag_errno = 0;
  }
  return response;
} /* END mcfg_fs_diag_put_handler */


/*
 * Populuates and returns the error respoonse packet.
 *
 * NOTE:The error passed should be the POSIX or extended errors
 * only this function will perform the mapping before sending the response.
 */
VOID_PACKED_PTR
mcfg_fs_diag_efs2_error_rsp (int32 diag_errno, VOID_PACKED_PTR req_pkt,
                        uint16 req_len)
{
  mcfg_fs_diag_error_rsp_type *response;
  unsigned int rsp_len;

  /* Lint is too stupid to realize that offsetof() has no side
   * effects, so it prohibits the use of the MIN() macro here (error
   * 666).  So we spell it out using small words instead. */
  rsp_len = req_len + offsetof (mcfg_fs_diag_error_rsp_type, pkt);
  if (rsp_len > sizeof (mcfg_fs_diag_error_rsp_type))
    rsp_len = sizeof (mcfg_fs_diag_error_rsp_type);

  response = (mcfg_fs_diag_error_rsp_type *)
    diagpkt_subsys_alloc (MCFG_SUBSYS,
                          MCFG_FS_DIAG_ERR,
                          rsp_len);
  if (response == NULL)
    return NULL;

  response->diag_errno = mcfg_fs_get_diag_errno (diag_errno);

  memscpy ((void *) response->pkt,
              rsp_len - offsetof (mcfg_fs_diag_error_rsp_type, pkt),
              (void *) req_pkt,
              rsp_len - offsetof (mcfg_fs_diag_error_rsp_type, pkt));

  return ((void *) response);
}/* END mcfg_fs_diag_efs2_error_rsp */


#endif /* FEATURE_MCFG_DIAG_SUPPORT */
