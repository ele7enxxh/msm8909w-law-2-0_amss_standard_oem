/** @addtogroup TLAPI_ERR
 * @{
 * TlApi return and error values.
 * Generic error codes valid for all TlApi functions are defined here.
 *
 * @file
 * TlApi return and error values.
 * Generic error codes valid for all TlApi functions are defined here.
 *
 * Copyright (c) 2013 TRUSTONIC LIMITED
 * All rights reserved
 * 
 * The present software is the confidential and proprietary information of
 * TRUSTONIC LIMITED. You shall not disclose the present software and shall
 * use it only in accordance with the terms of the license agreement you
 * entered into with TRUSTONIC LIMITED. This software may be subject to
 * export or import laws in certain countries.
 */
#ifndef __TLAPIERROR_H__
#define __TLAPIERROR_H__

/*
 *  TLAPI error codes.
 *  MAJOR part of error code is stable.
 *  MAJOR part may be used in testing for specific error code.
 *
 *  Detail error codes may change in different releases
 *  Please do not test DETAIL part when comparing error codes.
 */
#define TLAPI_ERROR_MAJOR(ecode)            ((ecode)       & 0xFFF) /**< Get MAJOR part of error code. */
#define TLAPI_ERROR_MAJOR_CODE(ecode)       ((ecode)       & 0xFF)  /**< Get MAJOR_CODE part of error code. */
#define TLAPI_ERROR_MAJOR_COMPONENT(ecode)  (((ecode)>>8)  & 0xF)   /**< Get MAJOR_COMPONENT part of error code. */

//
#define TLAPI_ERROR_DETAIL(ecode)           (((ecode)>>12) & 0xFFF) /**< Get detail part of error code. */


#define TLAPI_ERROR_CREATE(ecode, detail)   (((ecode)&0xFFF)|((detail&0xFFF)<<12)) /**< Create error code */


typedef unsigned int tlApiResult_t;

/** TlApi return values. */
#define TLAPI_OK                            0x00000000    /**< Returns on successful execution of a function. */

// TLApi generic errors
#define E_TLAPI_NOT_IMPLEMENTED             0x00000101    /**< Function not yet implemented. */
#define E_TLAPI_UNKNOWN                     0x00000102    /**< Unknown error during TlApi usage. */
#define E_TLAPI_UNKNOWN_FUNCTION            0x00000104    /**< Function not known. */
#define E_TLAPI_INVALID_INPUT               0x00000105    /**< Input data is invalid. */
#define E_TLAPI_INVALID_RANGE               0x00000106    /**< If address/pointer. */
#define E_TLAPI_BUFFER_TOO_SMALL            0x00000107    /**< A buffer is too small. */
#define E_TLAPI_INVALID_TIMEOUT             0x00000108    /**< The chosen timeout value was invalid. */
#define E_TLAPI_TIMEOUT                     0x00000109    /**< Timeout expired. */
#define E_TLAPI_NULL_POINTER                0x0000010a    /**< Null pointer. */
#define E_TLAPI_INVALID_PARAMETER           0x0000010b    /**< Invalid parameter. */
#define E_TLAPI_UNMAPPED_BUFFER             0x0000010c    /**< Specified buffer is not entirely mapped in Trustlet address space. */
#define E_TLAPI_UNALIGNED_POINTER           0x0000010d    /**< Passed pointer is not word-aligned. */

// Communication error codes
#define E_TLAPI_COM_WAIT                    0x0000010e    /**< Waiting for a notification failed. */
#define E_TLAPI_COM_ERROR                   0x0000010f    /**< Internal communication error. */

// Memory type check error
#define E_TLAPI_BUFFER_INCORRECT_TYPE       0x00000110    /**< Passed buffer is not of correct type. */

// Error codes related to all drivers
#define E_TLAPI_DRV_UNKNOWN                 0x00000201    /**< Unspecified driver error. */
#define E_TLAPI_DRV_NO_SUCH_DRIVER          0x00000202    /**< Unknown driver, bad driver ID. */
#define E_TLAPI_DRV_INVALID_PARAMETERS      0x00000203    /**< Driver parameters invalid. */

// Crypto error codes
#define E_TLAPI_CR_HANDLE_INVALID           0x00000301    /**< No running session is associated with this handle value or caller has
                                                        not permission to access the session associated with this handle value. */
#define E_TLAPI_CR_ALGORITHM_NOT_AVAILABLE  0x00000302    /**< Algorithm is not available for the caller. */
#define E_TLAPI_CR_ALGORITHM_NOT_SUPPORTED  0x00000303    /**< The intended algorithm usage is not supported. */
#define E_TLAPI_CR_WRONG_INPUT_SIZE         0x00000304    /**< Input data (message or data to be encrypted or decrypted) is too short or too long. */
#define E_TLAPI_CR_WRONG_OUTPUT_SIZE        0x00000305    /**< Provided Output buffer is of wrong size. */
#define E_TLAPI_CR_WRONG_OUPUT_SIZE         E_TLAPI_CR_WRONG_OUTPUT_SIZE /**< for backward compatibility */
#define E_TLAPI_CR_INCONSISTENT_DATA        0x00000306    /**< Inconsistency of data was determined. */
#define E_TLAPI_CR_OUT_OF_RESOURCES         0x00000307    /**< No more additional resources available. */

// Secure object codes.
#define E_TLAPI_SO_WRONG_CONTEXT            0x00000401    /**< Illegal (unsupported) secure object context. */
#define E_TLAPI_SO_WRONG_PADDING            0x00000402    /**< Wrong padding of secure object. */
#define E_TLAPI_SO_WRONG_CHECKSUM           0x00000403    /**< Wrong secure object checksum. */
#define E_TLAPI_SO_CONTEXT_KEY_FAILED       0x00000404    /**< Derivation of context key failed. */
#define E_TLAPI_SO_WRONG_LIFETIME           0x00000405    /**< Illegal (unsupported) secure object lifetime. */
#define E_TLAPI_SO_WRONG_VERSION            0x00000406    /**< Illegal (unsupported) secure object version. */
#define E_TLAPI_SO_WRONG_TYPE               0x00000407    /**< Illegal (unsupported) secure object type. */
#define E_TLAPI_SO_DELEGATED_NOT_PERMITTED  0x00000408    /**< Unwrap does not permit delegated objects. */
#define E_TLAPI_SO_CONTEXT_NOT_PERMITTED    0x00000409    /**< Unwrap does not permit this context. */


#endif // __TLAPIERROR_H__

/** @} */
