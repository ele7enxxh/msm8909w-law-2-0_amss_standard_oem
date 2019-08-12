//
// NOTE: No implementation exists based on this header.
//
///** TLAPI_KPD
// *
// * Basic secure keypad functions.
// *
// * The secure keypad interface provides functions to read input from a simple keypad of the device.
// * Input will be transmitted in a secure way to the Trustlet. During the read operations the keypad
// * is disconnected from the non-secure world.
// *
// *
// * The secure keypad interface provides functions to read input from a simple keypad of the device.
// * Input will be transmitted in a secure way to the Trustlet. During the read operations the keypad
// * is disconnected from the non-secure world.
// *
// * Copyright (c) 2013 TRUSTONIC LIMITED
// * All rights reserved
// * 
// * The present software is the confidential and proprietary information of
// * TRUSTONIC LIMITED. You shall not disclose the present software and shall
// * use it only in accordance with the terms of the license agreement you
// * entered into with TRUSTONIC LIMITED. This software may be subject to
// * export or import laws in certain countries.
// */
////#ifndef __TLAPIKEYPAD_H__
////#define __TLAPIKEYPAD_H__
//
//#include "TlApi/TlApiCommon.h"
//#include "TlApi/TlApiError.h"
//
///** Reserve the keypad for the caller.
// * With tlApiGrabKeypad() a Trustlet is able to reserve the keypad for reading more than one
// * charater. The keypad will remain reserved for the caller as long as the caller does not
// * release it using tlApiReleaseKeypad().
// * If the keypad is already grabbed by the caller the function returns TLAPI_OK.
// *
// * return TLAPI_OK if keyboard has been grabbed
// */
//_TLAPI_EXTERN_C tlApiResult_t tlApiGrabKeypad(
//    void);
//
///** Read a character from the keypad.
// * The function waits for a keypress and returns the character associated with the key
// * which has been pressed. If multiple key presses shall be captured (e.g. during pin entry)
// * the keypad should be grabbed beforehand and released after the operation is complete.
// * Depending on the underlying hardware platform a wait timeout may be provided. If the waiting timeout is not supported by
// * the platform E_TLAPI_INVALID_TIMEOUT is returned.
// *
// * param c         Pointer to the character.
// * param timeout   Time in milliseconds to wait (TLAPI_NO_TIMEOUT => direct return, TLAPI_INFINITE_TIMEOUT => wait infinitely
// * return TLAPI_OK if character c has successfully been read.
// */
//_TLAPI_EXTERN_C tlApiResult_t tlApiGetC(
//    uint32_t   *c,
//    uint32_t   timeout);
//
///** Releases the grabbed keypad.
// * tlApiReleaseKeypad() releases the keypad which has been grabbed by the Trustlet before.
// * If the caller does currently not own the keypad the function returns TLAPI_OK.
// *
// *  return TLAPI_OK if operation has been successfully completed
// */
//_TLAPI_EXTERN_C tlApiResult_t tlApiReleaseKeypad(
//    void);
//
//#endif // __TLAPIKEYPAD_H__
//
///**  */
