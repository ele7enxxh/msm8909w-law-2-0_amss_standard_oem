#ifndef IXERRNO_H
#define IXERRNO_H

/**
  @file IxErrno.h
  @brief This contains the definition of the return codes (error numbers).
  Functions using this definition either return an error code, or set
  a global variable to the appropriate value.

*/
/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The Utility_Services_API_mainpage.dox file contains all file/group 
      descriptions that are in the output PDF generated using Doxygen and 
      Latex. To edit or update any of the file/group text in the PDF, edit 
      the Utility_Services_API_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the "utils_error_codes" 
	  group description in the Utility_Services_API_mainpage.dox file. 
===========================================================================*/
/*===========================================================================

               E R R O R   N U M B E R   D E F I N I T I O N S

DESCRIPTION
  This contains the definition of the return codes (error numbers).
  Functions using this definition either return an error code, or set
  a global variable to the appropriate value.
 

Copyright (c) 2006-2011 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary. 
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //components/rel/core.mpss/3.5.c12.3/api/services/IxErrno.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/15/11   llg     (Tech Pubs) Edited/added Doxygen comments and markup.
11/12/10   EBR     Doxygenated File.
10/28/09   chm     Add support for more informational return values from 
                   WMDRM.
02/20/06   ssm     Deleted IxErrno in Interfaces dir and copied to services
                   folder
01/03/06   ssm     Cloned from errno.h

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

/*===========================================================================

              DEFINITIONS AND CONSTANTS FOR ERROR CODES

===========================================================================*/
/** @addtogroup utils_error_codes
@{ */

/** @brief Types of return codes. */
typedef enum
{
  /* Generic outcomes of operations */
  E_SUCCESS            =  0,    /**< Operation was successful. */
  E_FAILURE            =  1,    /**< Operation failed due to an unknown 
                                     error. */
  E_NOT_ALLOWED        =  2,    /**< Operation currently is not allowed. */
  E_NOT_AVAILABLE      =  3,    /**< Operation currently is not available. */
  E_NOT_SUPPORTED      =  4,    /**< Operation is not yet implemented. */
  E_CANCELED           =  5,    /**< Operation was scheduled but canceled.  */
  E_ABORTED            =  6,    /**< Operation was started but aborted. */
  E_INTERRUPTED        =  7,    /**< Operation was started but interrupted. */
  E_DEADLOCK           =  8,    /**< Operation would cause a deadlock. */
  E_AGAIN              =  9,    /**< Caller should retry the operation. */
  E_RESET              = 10,    /**< Executing module was reset. */
  E_WOULD_BLOCK        = 11,    /**< Nonblocking I/O operation would block. */
  E_IN_PROGRESS        = 12,    /**< Operation is in progress. */
  E_ALREADY_DONE       = 13,    /**< Operation is already completed. */ 

  /* Memory management-related error conditions. */
  E_NO_DSM_ITEMS       = 14,    /**< Memory management-related: out of DSM items. */
  E_NO_MEMORY          = 15,    /**< Memory management-related: allocation from 
                                     a memory pool failed. */

  /* Parameter or data parsing-related error conditions. */
  E_INVALID_ARG        = 16,    /**< Parameter or data parsing-related: 
                                     argument is not recognized. */
  E_OUT_OF_RANGE       = 17,    /**< Parameter or data parsing-related: 
                                     argument value is out of range. */
  E_BAD_ADDRESS        = 18,    /**< Parameter or data parsing-related: 
                                     pointer argument is a bad address. */
  E_NO_DATA            = 19,    /**< Parameter or data parsing-related: 
                                     expected data, but received none. */
  E_BAD_DATA           = 20,    /**< Parameter or data parsing-related: 
                                     data failed a sanity check, e.g., CRC. */
  E_DATA_INVALID       = 21,    /**< Parameter or data parsing-related: 
                                     data is correct, but the contents are 
                                     invalid. */
  E_DATA_EXPIRED       = 22,    /**< Parameter or data parsing-related: 
                                     data is not yet or is not valid any more. */
  E_DATA_TOO_LARGE     = 23,    /**< Parameter or data parsing-related: 
                                     data is too large to process. */

  /* Hardware-related error conditions. \n */
  E_NO_DEV             = 24,    /**< Hardware-related: hardware resource 
                                     is not available. */
  E_DEV_FAILURE        = 25,    /**< Hardware-related: hardware failure 
                                     occurred. */
  E_NV_READ_ERR        = 26,    /**< Hardware-related: operation failed 
                                     due to an NV read error. */
  E_NV_WRITE_ERR       = 27,    /**< Hardware-related: operation failed 
                                     due to an NV write error. */
  E_EFS_ERROR          = 28,    /**< Hardware-related: embedded file system 
                                     interface error. */
  E_DSP_ERROR          = 29,    /**< Hardware-related: DSP returned an error 
                                     on command (-1). */

  /* Protocol operation-related error conditions. */
  E_TIMER_EXP          = 30,    /**< Protocol operation-related: operation 
                                     is not completed (timer expiration). */
  E_VERSION_MISMATCH   = 31,    /**< Protocol operation-related: unexpected 
                                     software or protocol version.  */
  E_TASK_SIG_ERR       = 32,    /**< Protocol operation-related: signal is 
                                     unknown or there is no handler. */
  E_TASK_Q_FULL        = 33,    /**< Protocol operation-related: task queue 
                                     is full. */
  E_PROT_Q_FULL        = 34,    /**< Protocol operation-related: protocol 
                                     queue is full. */
  E_PROT_TX_Q_FULL     = 35,    /**< Protocol operation-related: protocol Tx 
                                     data queue is full. */
  E_PROT_RX_Q_FULL     = 36,    /**< Protocol operation-related: protocol Rx 
                                     data queue is full. */
  E_PROT_UNKN_CMD      = 37,    /**< Protocol operation-related: protocol 
                                     does not understand the command. */
  E_PROT_UNKN_IND      = 38,    /**< Protocol operation-related: protocol 
                                     does not understand the indicator. */
  E_PROT_UNKN_MSG      = 39,    /**< Protocol operation-related: protocol 
                                     does not understand the message. */
  E_PROT_UNKN_SIG      = 40,    /**< Protocol operation-related: protocol 
                                     does not understand the signal. */

  /* File-related error conditions. */
  E_NO_ENTRY           = 41,    /**< File-related: no such file or directory. */
  E_FILENAME_TOO_LONG  = 42,    /**< File-related: file name is too long. */
  E_DIR_NOT_EMPTY      = 43,    /**< File-related: directory is not empty. */
  E_IS_DIR             = 44,    /**< File-related: is a directory. */
  E_EOF                = 45,    /**< File-related: end of the file. */
  E_XDEV               = 46,    /**< File-related: attempt to cross a device. */
  E_BAD_F              = 47,    /**< File-related: bad file descriptor. */
  E_MAX_OPEN_FILES     = 48,    /**< File-related: too many open files. */
  E_BAD_FMT            = 49,    /**< File-related: bad formatted media. */
  E_FILE_EXISTS        = 50,    /**< File-related: file already exists. */
  E_EFS_FULL           = 51,    /**< File-related: no space is left on the 
                                     device. */
  E_NOT_DIR            = 52,    /**< File-related: not a directory. */
  E_NOT_EFS_ITEM       = 53,    /**< File-related: not an EFS2 item. */
  E_ALREADY_OPEN       = 54,    /**< File-related: file/directory is already 
                                     open. */
  E_BUSY               = 55,    /**< File-related: device/item is busy. */
  E_OUT_OF_NODES       = 56,    /**< File-related: too many symbolic links were 
                                     encountered. */

  /* SQL-related error conditions. */
  E_SQL_INTERNAL       = 57,    /**< SQL-related: internal logic error in SQLite. */
  E_SQL_BUSY           = 58,    /**< SQL-related: database file is locked. */
  E_SQL_LOCKED         = 59,    /**< SQL-related: table in the database is locked. */
  E_SQL_READONLY       = 60,    /**< SQL-related: attempt to write to a read-only 
                                     database. */
  E_SQL_IOERR          = 61,    /**< SQL-related: disk I/O error occurred. */
  E_SQL_CORRUPT        = 62,    /**< SQL-related: database disk image is 
                                     malformed. */
  E_SQL_NOTFOUND       = 63,    /**< SQL-related: table or record was not found. */
  E_SQL_FULL           = 64,    /**< SQL-related: insertion failed because the 
                                     database is full. */
  E_SQL_CANTOPEN       = 65,    /**< SQL-related: unable to open the database 
                                     file. */
  E_SQL_PROTOCOL       = 66,    /**< SQL-related: database lock protocol error. */
  E_SQL_EMPTY          = 67,    /**< SQL-related: database is empty. */
  E_SQL_SCHEMA         = 68,    /**< SQL-related: database schema has changed. */
  E_SQL_CONSTRAINT     = 69,    /**< SQL-related: abort due to a constraint 
                                     violation. */
  E_SQL_MISMATCH       = 70,    /**< SQL-related: data type mismatch. */
  E_SQL_MISUSE         = 71,    /**< SQL-related: library was used incorrectly. */
  E_SQL_NOLFS          = 72,    /**< SQL-related: OS features are not supported. */
  E_SQL_FORMAT         = 73,    /**< SQL-related: auxiliary database format error. */
  E_SQL_NOTADB         = 74,    /**< SQL-related: file opened is not a database 
                                     file. */
  E_SQL_ROW            = 75,    /**< SQL-related: sqlite3_step() has another 
                                     row ready. */
  E_SQL_DONE           = 76,    /**< SQL-related: sqlite3_step() has finished 
                                     executing. */
  E_NO_ATTR            = 77,    /**< SQL-related: no attribute found. */
  E_INVALID_ITEM       = 78,    /**< SQL-related: invalid item found. */

  /* Network-related error conditions. */
  E_NO_NET             = 100,   /**< Network-related: network is not 
                                     connected (i.e., is not up). */
  E_NOT_SOCKET         = 101,   /**< Network-related: socket operation is on 
                                     a nonsocket. */
  E_NO_PROTO_OPT       = 102,   /**< Network-related: network protocol is 
                                     not available. */
  E_SHUTDOWN           = 103,   /**< Network-related: socket is being shut down. */
  E_CONN_REFUSED       = 104,   /**< Network-related: connection was refused by 
                                     a peer. */
  E_CONN_ABORTED       = 105,   /**< Network-related: connection was aborted. */
  E_IS_CONNECTED       = 106,   /**< Network-related: connection is already 
                                     established. */
  E_NET_UNREACH        = 107,   /**< Network-related: network destination is 
                                     unreachable. */
  E_HOST_UNREACH       = 108,   /**< Network-related: host destination is 
                                     unreachable. */
  E_NO_ROUTE           = 109,   /**< Network-related: no route to the destination. */
  E_ADDR_IN_USE        = 110,   /**< Network-related: IP address is already 
                                     in use. */

  /* BCMCS-related error conditions. */
  E_DB_INVALID         = 111,   /**< BCMCS-related: entire database is invalid. */
  E_FLOW_NOT_IN_ZONE   = 112,   /**< BCMCS-related: flow is not in the specified 
                                     zone. */
  E_ZONE_NOT_IN_DB     = 113,   /**< BCMCS-related: specified zone is not in 
                                     the database. */
  E_BUF_OVERFLOW       = 114,   /**< BCMCS-related: output buffer is too small. */
  //E_EOF              = 115,   /**< BCMCS-related: end of the file was reached. */
  E_DB_OVERFLOW        = 116,   /**< BCMCS-related: database is full; no further 
                                     updates. */
  E_DB_NO_OVERWRITE    = 117,   /**< BCMCS-related: value exists and the 
                                     overwrite is false. */
  E_NOT_FOUND          = 118,   /**< BCMCS-related: data was not found. */
  E_NO_RIGHTS          = 119,   /**< BCMCS-related: no rights/permission to 
                                     access the data. */
  E_HASH_VERIF_FAIL    = 120,   /**< BCMCS-related: hash verification on the 
                                     DCF failed. */

  /* WMDRM-related error conditions. */
  E_INVALID_STORE_PSWD  = 121,  /**< WMDRM-related: invalid secure store password. */
  E_MACHINE_ID_MISMATCH = 122,  /**< WMDRM-related: machine ID mismatch. */
 
  /* HTTP-related error conditions. */
  E_CONTINUE               = 200,   /**< HTTP-related: Server response status 
                                         code. */

  /** @cond */
  /* note: Values to be deleted */
  IxSUCCESS           =  E_SUCCESS,
  IxFAILED,
  IxNOMEMORY, 
  IxBADPARAM,
  IxNOTSUPPORTED,

  E_RESERVED           = 0x7FFFFFFF
  /** @endcond */
} IxErrnoType;

/** Backwards compatibility. */
typedef IxErrnoType errno_enum_type;

/** @} */ /* end_addtogroup utils_error_codes */
#endif /* IXERRNO_H */
