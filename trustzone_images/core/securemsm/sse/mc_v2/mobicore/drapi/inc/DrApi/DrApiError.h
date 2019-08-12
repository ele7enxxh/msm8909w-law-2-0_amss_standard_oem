/** @addtogroup DRAPI_ERR
 * @{
 * 
 * @file DrApiError.h
 * @brief  Secure Driver API return and error values.
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
#ifndef __DRAPIERROR_H__
#define __DRAPIERROR_H__

/*
 *  DRAPI error codes.
 *  MAJOR part of error code is stable.
 *  MAJOR part may be used in testing for specific error code.
 *
 *  Detail error codes may change in different releases
 *  Please do not test DETAIL part when comparing error codes.
 */
#define DRAPI_ERROR_MAJOR(ecode)            ((ecode)       & 0xFFF) /**< Get MAJOR part of error code. */
#define DRAPI_ERROR_MAJOR_CODE(ecode)       ((ecode)       & 0xFF)  /**< Get MAJOR_CODE part of error code. */
#define DRAPI_ERROR_MAJOR_COMPONENT(ecode)  (((ecode)>>8)  & 0xF)   /**< Get MAJOR_COMPONENT part of error code. */

#define DRAPI_ERROR_DETAIL(ecode)           (((ecode)>>12) & 0xFFF) /**< Get detail part of error code. */

#define DRAPI_ERROR_CREATE(ecode, detail)   (((ecode)&0xFFF)|((detail&0xFFF)<<12)) /**< Create error code */

//==============================================================================
// Error code defintitions

#define DRAPI_OK                    0x0      /**< Returns on successful execution of a function. */

#define E_DRAPI_KERNEL_ERROR        0xF01    /**< Kernel returned error. */
#define E_DRAPI_INVALID_PARAMETER   0xF02    /**< Invalid parameter. */
#define E_DRAPI_NOT_PERMITTED       0xF03    /**< Permission error */
#define E_DRAPI_IPC_ERROR           0xF04    /**< Error in IPC. */
#define E_DRAPI_TASK_NOT_ACCEPTABLE 0xF05    /**< Task not acceptable for operation. */
#define E_DRAPI_CANNOT_MAP          0xF06    /**< Cannot create mapping. */
#define E_DRAPI_DRV_NO_SUCH_CLIENT  0xF07    /**< Client does not exist. */
#define E_DRAPI_CANNOT_INIT         0xF08    /**< Cannot be initialized. */
#define E_DRAPI_NOT_IMPLEMENTED     0xF09    /**< Function not yet implemented. */


// Error codes inherited from MTK 
#define E_OK						0	/**< no error. */
#define E_INVALID					1	/**< invalid argument. */
#define E_BADTASK					2	/**< current task does not own target task. */
#define E_NOTACTIVATED				3	/**< task has not been activated. */
#define E_NOTOWNER					4	/**< current task does not own specified task. */
#define E_ACTIVATED					5	/**< task has been activated. */
#define E_LIMIT						6	/**< limit broken. */
#define E_NOABILITY					7	/**< no permission. */
#define E_STARTED					8	/**< task or thread does have been started. */
#define E_BADMAP					9	/**< invalid mapping. architecture specific error. */
#define E_MAPPED					10	/**< mapping overlaps existing mapping. */
#define E_NOTSTARTED				11	/**< thread has been started. */
#define E_TIMEOUT					12	/**< timeout period expired. */
#define E_ABORT						13	/**< operation aborted. */
#define E_MSGTYPE					14	/**< message to send is not of the type the receiver is waiting for. */
#define E_MSGLENGTH					15	/**< message to send exceeds message length the receiver is waiting for. */


#endif // __DRAPIERROR_H__

/** @} */
