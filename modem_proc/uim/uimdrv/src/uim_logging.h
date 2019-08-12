#ifndef UIM_LOGGING_H
#define UIM_LOGGING_H
/*==============================================================================
  FILE:         uim_logging.h

  OVERVIEW:     FIle conatins the inclusions and definitions needed for
                logging related functions.

  DEPENDENCIES: N/A

                Copyright (c) 2014-2015 QUALCOMM Technologies, Inc.
                All Rights Reserved.
                QUALCOMM Technologies Confidential and Proprietary
==============================================================================*/

/*=============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/uimdrv/src/uim_logging.h#1 $
$DateTime: 2016/12/13 08:00:05 $
$Author: mplcsds1 $

when        who        what, where, why
------      ----       ---------------------------------------------------------
05/10/16    sam        Remove the logic to store timeout info in EFS
04/28/16    na         Restricting EFS APDU Logging with Feature
12/22/14    na         Making APDU Logging file instance based
04/29/14    nmb        Correct LOG_UIM_MAX_CHARS to pass buffer storage checks
01/23/14    yk         The first revision
==============================================================================*/


/* Number of UIM GPIOs as inputs upon timeout w/recovery disabled*/
#define UIM_NUM_TIMEOUT_INFO                                    10

#define UIM_EFSLOG_MAX_SAVES_TO_EFS 100

/* Max number of bytes can be logged */
#define LOG_UIM_MAX_CHARS   246

/* Time stamp size */
#define LOG_UIM_TSTAMP_SIZE  8

/* EFS LOG File SIZE & RAM Buffer Size */
#define UIM_EFSLOG_FILE_SIZE        1500
#define UIM_EFSLOG_MAX_BUFFER_SIZE  1000
#define UIM_EFSLOG_ASCII_CR         0x0D
#define UIM_EFSLOG_ASCII_LF         0x0A


/* Define data structure */
LOG_RECORD_DEFINE(LOG_UIM_DATA_C)
  byte  length;  /* Actual data length */
  byte  data[LOG_UIM_MAX_CHARS + LOG_UIM_TSTAMP_SIZE];
LOG_RECORD_END

typedef LOG_UIM_DATA_C_type uim_log_data_type;

/* Attribute of followed data */
typedef enum {
  UIM_LOG_ATTRIB_INIT  = 0x00, /* Init value */
  UIM_LOG_TSTAMP       = 0x01, /* Data is time stamp */
  UIM_LOG_TX_DATA      = 0x10, /* Data is ME to UIM */
  UIM_LOG_TX_REPEAT    = 0x30, /* Re-sent data from ME to UIM */
  UIM_LOG_RX_DATA      = 0x80, /* Data is UIM to ME */
  UIM_LOG_RX_REPEAT    = 0xC0  /* Re-sent data from UIM to ME */
} uim_log_attrib_type;

/* Type for logging character used in logging */
typedef struct
{
  uim_log_attrib_type     attrib;
  uim_instance_enum_type  slot_id;
  char                    the_char;
} uim_log_char_type;

/* Type for log mask used for apdu logging*/
typedef enum
{
  INVALID_LOGGING_MASK = 0,
  SINGLE_SLOT_LOGGING_MASK_1098,
  MULTI_SLOT_LOGGING_MASK_14CE
}uimLogMaskType;

typedef struct
{
  uimLogMaskType  log_mask;
}uim_apdu_log_mask_type;

typedef struct {
  char *data;
  int  buffer_size;
} uim_efslog_buffer_type;

/**
 * Put a byte to APDU log buf
 *
 * @param attr Attribute of logging data
 * @param ch Data to be logged.
 * @param uim_ptr Pointer to the global data common.
 */
void uim_log_put_byte(uim_log_attrib_type attr,
                      char ch,
                      uim_instance_global_type *uim_ptr);

/**
 * Put a time stamp then logging all data in APDU log bug.
 *
 * @param uim_ptr
 */
void uim_log_put_last_byte(uim_instance_global_type *uim_ptr);

/**
 * Save GPIO register state to globals upon command response
 * timeout(Saves the 10 most recent occurances of command
 * response timeout on an internal or external command).
 *
 * @param uim_ptr Pointer to the global data common.
 * @param is_internal_command Internal or External.
 */
void uim_save_gpio_info
(
  uim_instance_global_type *uim_ptr,
  boolean                  is_internal_command
);

/**
 * This file establishes the file in EFS to which EFSLOG will
 * write. The local EFSLOG buffer cannot be purged until this
 * function has completed successfully. This function will also
 * establish the local EFS buffer
 *
 * @param uim_ptr Pointer to the global data common.
 *
 * @return boolean Success or Fail.
 */
boolean uim_efslog_init( uim_instance_global_type *uim_ptr );

/**
 * This function takes a character to be stored to the buffer
 * and an attribute,only if EFS Logging is currently in
 * progress. It converts the character to ascii, and checks the
 * attribute to see if it differs from the previous character's
 * attribute. If it does not, then it simply places the ASCII
 * value of the character in the next space in the buffer (One
 * character will become two ASCII bytes).  If the attribute is
 * different, it will place a new line character '\n' followed
 * by the attribute tag, a colon, and then the character (in
 * ASCII format).
 *
 * @param attribute Attribute of logging data
 * @param character Data to be logged.
 * @param uim_ptr Pointer to the global data common.
 */
void uim_efslog_apdu_to_buffer(
  uim_instance_global_type *uim_ptr
);

/**
 * This function gets called when a the local logging buffer
 * reaches a certain threashold, and we want to purge this data
 * to the EFS file.  This makes a blocking call to efs_write.
 * First, we copy into the secondary buffer and then write the
 * contents of the secondary buffer to EFS.
 *
 * @param uim_ptr Pointer to the global data common.
 */
void uim_efslog_save_buffer_to_efs( uim_instance_global_type *uim_ptr );

/**
 * Cleans up EFSLOG when complete
 *
 * @param uim_ptr
 */
void uim_efslog_clean_up( uim_instance_global_type *uim_ptr);

/**
 * Processing of PURGE SIGNAL
 *
 * @param uim_ptr
 */
void uim_efslog_process_purge_sig(uim_instance_global_type *uim_ptr);


#endif /* UIM_LOGGING_H */
