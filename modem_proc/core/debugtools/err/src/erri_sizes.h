#ifndef ERRI_SIZES_H
#define ERRI_SIZES_H

/*===========================================================================

                    Error Handling Service Internal Header File

Description

Copyright (c) 2015 by Qualcomm Technologies Incorporated.  All Rights Reserved.

$Header: //components/rel/core.mpss/3.5.c12.3/debugtools/err/src/erri_sizes.h#1 $

===========================================================================*/

/* Used in erri.h */
#define ERR_SSR_REASON_SIZE_BYTES            80

/* Used in errlog.c */
#define ERR_STACK_DUMP_SIZE                0x64

/* Used in errlog.h */
#define ERR_LOG_MAX_MSG_LEN                 80

/* Used in errlog.h */
#define ERR_LOG_MAX_FILE_LEN                50 

/* Used in errlog.h */
#define ERR_IMAGE_VERSION_SIZE             128  

/* Used in errlog.h */
/* 20 for modem, otherwise 5 */
#ifndef ERR_EXTEND_PREFLUSH_CB
#define ERR_MAX_PREFLUSH_CB                 5
#elif   ERR_EXTEND_PREFLUSH_CB
#define ERR_MAX_PREFLUSH_CB                 20
#endif /* ERR_MAX_PREFLUSH_CB */

/* Used in err.c */
#define ERR_DYNAMIC_MSG_SIZE ( MIN(ERR_SSR_REASON_SIZE_BYTES, ERR_LOG_MAX_MSG_LEN) )

/* Used in err_exception_handler.c\err_pd_exception_handler.c */
#define LEN_QXDM_DBG_MSG                   80

/* Used in err_qdi.h */
#define ERR_QDI_PFR_LEN                    80

/* Used in err_types.h */
#define ERR_FILE_NAME_LEN                   8

/* Used in err_types.h */
/* Maximum size of the extended info written out to SMEM */
#define ERR_DATA_MAX_SIZE              0x4000

/* Used in uerr_excepion_handler.c */
#define UERR_TASK_STACK_SIZE              768

/* Used in err_decompress.c */
/* Max chunk size to decompress into
  implementation borrowed from http://zlib.net/zlib_how.html */
#define ERR_DECOMP_CHUNK                 4096

/* Used in err_nv_log.c */
/* NV stores 20 error logs */
#define ERR_MAX_LOG                        20


#endif /* ERRI_SIZES_H */
