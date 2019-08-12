/*
 * Copyright (c) 2012-2013 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef TLP11_UTILS_H
#define TLP11_UTILS_H

#include <stddef.h>
#include "stringl.h"
#include "tlP11Objects.h"
#include "tlP11TypesInt.h"
#include <TlApi/TlApi.h>

#ifndef min
#define min(a,b) ((a)<(b)?(a):(b))
#endif
#ifndef max
#define max(a,b) ((a)>(b)?(a):(b))
#endif

#define RETURN_CHAR '\n'
#define ESCAPE_CHAR 0x1B

/** Verify buffer is outside the reserved area in TL address space.
 * Verify that the buffer is outside of the memory area
 * reserved for the CODE+DATA+BSS for the running TL. */
CK_RV validateBufferLocation(void *ptr);

/** Convert a TlApi error code in the equivalent PKCS#11 code */
CK_RV TL2CK(tlApiResult_t err);

/** Set an attribute template to CK_UNAVAILABLE_INFORMATION */
CK_RV zeroAttributeTemplate(tlp11_attribute_template_t *templ);

/** Get a PIN from the secure keypad */
CK_RV getPIN(
    const uint32_t min,
    const uint32_t max,
    UTF8CHAR *pin,
    uint32_t *pinLen,
    tlp11_keypad_event_t *event,
    uint32_t *pinFailed
    );

/** Verify and save the newly entered PIN */
CK_RV verifySavePIN(
    uint32_t *pinFailed,
    UTF8CHAR  *pin,
    uint32_t *pinLen,
    UTF8CHAR  *pinVerification,
    uint32_t *pinVerLen,
    tlp11_token_t *token
    );

/** Enforce a policy on authentication failure */
void onAuthFailed(const CK_ULONG ret);

/** Compute the CKA_CHECK_VALUE checksum on a key */
tlApiResult_t computeChecksum(
    uint8_t *checksum,
    uint8_t *src,
    const uint32_t len,
    const tlp11_object_type_enum_t objType);

/** Wrap a generic object.
 * The boundary of the plain text section is determined by the structure of the object itself
 * using the \c so_enc_start element.
 *
 */
tlApiResult_t wrapObject(
    tlp11_container_object_t *object,
    tlp11_container_object_so_t *so_object);

/** Unwrap a generic object.
 */
tlApiResult_t unwrapObject(
    const tlp11_container_object_so_t *container,
    tlp11_container_object_t *destination);

/** Wrap a token.
 * The boundary of the plain text section is determined by the structure of the object itself
 * using the \c so_enc_start element.
 */
tlApiResult_t wrapToken(
    tlp11_token_t *token,
    tlp11_token_so_t *so_token);

/** UnWrap a token.
 */
tlApiResult_t unwrapToken(
    const tlp11_token_so_t *so_token,
    tlp11_token_t *token);

/** Wrap a group of sessions.
 * The group of sessions is wrapped as plaintext
 */
tlApiResult_t wrapSessionGroup(
    tlp11_session_group_t *group,
    tlp11_session_group_so_t *so_group);

/** UnWrap a group of sessions.
 */
tlApiResult_t unwrapSessionGroup(
    const tlp11_session_group_so_t *so_group,
    tlp11_session_group_t *group);

/** Wrap a time.
 * The group of sessions is wrapped as plaintext
 */
tlApiResult_t wrapTime(
    const tlp11_lock_time_t *time,
    tlp11_lock_time_so_t *so_time);

/** UnWrap a group of sessions.
 */
tlApiResult_t unwrapTime(
    const tlp11_lock_time_so_t *so_time,
    tlp11_lock_time_t *time);

/** Try to unlock a token
 */
CK_FLAGS unlockToken(tlp11_token_t *token);

/** Lock a token
 */
CK_RV lockToken(tlp11_token_t *token, CK_USER_TYPE user);

/** Check if the requested operation can be allowed.
 *
 * Returns an appropriate error code if the operation cannot be allowed, or
 * CKR_OK otherwise.
 * As side effects of this operation, the current session is identified and
 * returned, and the authentication state of the token (session group) can
 * change, if the max number of operations has been reached, or the
 * authentication timeout has expired.
 */
CK_RV checkAccess(
    const tlp11_token_t *token,
    tlp11_session_group_t *group, // not const, auth info might change
    const uint32_t index,
    const uint32_t access
    );

CK_RV getSessionCount(
    const tlp11_session_group_t *group,
    uint32_t *sessionCount,
    uint32_t *rwSessionCount,
    uint32_t *index // returned, first empty one
    );

CK_RV processTokenGroup(
    const tlp11_token_so_t *so_token,
    const tlp11_session_group_so_t *so_group,
    tlp11_token_t *token,
    tlp11_session_group_t *group
    );

CK_RV processTokenGroupSession(
    const tlp11_token_so_t *so_token,
    const tlp11_session_group_so_t *so_group,
    const uint32_t sessionId,
    tlp11_token_t *token,
    tlp11_session_group_t *group,
    uint32_t *index
    );

CK_RV processTokenGroupSessionObject(
    const tlp11_token_so_t *so_token,
    const tlp11_session_group_so_t *so_group,
    const uint32_t sessionId,
    const tlp11_container_object_so_t *so_object,
    tlp11_token_t *token,
    tlp11_session_group_t *group,
    uint32_t *index,
    tlp11_container_object_t *object
    );

/** Mechanism is among the allowed ones for the key
 */
CK_RV mechanismIsAllowed(CK_MECHANISM_TYPE mechanism, const tlp11_key_t *key);

#ifdef TL_LOG
#define DUMP_HEX(A,B,C) dumpHex((A),(B),(C))
void dumpHex(const char *tag, const uint8_t *buffer, unsigned int len);
#else
#define DUMP_HEX(A,B,C) do {} while(0)
#endif

/** Fill in default values in a CK_TOKEN_INFO structure.
 * Used by token initialization functions and GetTokenInfo
 */
void fillTokenInfo(CK_TOKEN_INFO *info);

/** Complete a key wrapping after the mechanism initialization has been completed
 */
CK_RV completeWrapping(
    tlApiCrSession_t sessionHandle,
    tlp11_container_object_t *key,
    uint8_t *wrappedData,
    size_t *wrappedLen
    );

/** Complete a key unwrapping after the mechanism initialization has been completed
 */
CK_RV completeUnwrapping(
    tlApiCrSession_t sessionHandle,
    tlp11_container_object_t *newKey,
    const uint8_t *wrappedData,
    const size_t wrappedLen
    );

#ifdef SK_ENABLED
// stubbed functions for the secure keypad
inline tlApiResult_t tlApiGrabKeypad(void) {
  return E_TLAPI_UNKNOWN_FUNCTION;
}

inline tlApiResult_t tlApiGetC(uint32_t *c, uint32_t timeout){
  return E_TLAPI_UNKNOWN_FUNCTION;
}

inline tlApiResult_t tlApiReleaseKeypad(void) {
  return E_TLAPI_UNKNOWN_FUNCTION;
}
#endif
tlApiResult_t tlApiGetTimeStamp(
    timestamp_t *ts,
    tsSource_t  source
);


/**
  timesafe_memcmp - Constant-time memory comparison

  Compares bytes at two different sections in memory in constant time

  This function compares the len bytes starting at ptr1 with the len
  bytes starting at ptr2.  The function returns 1 if the two sections
  of memory are different and 0 if the two sections of memory are
  identical.  The function always scans the entire range of memory to
  ensure the function runs in constant time.

  This function should be used when comparing confidential information
  in memory as it prevents timing attacks.  A traditional memcmp() exits
  after finding non-equal bytes and this can be used to determine the value
  of confidential data in memory.  Examples uses include password checks,
  MACs checks, decryption checks, and checks on private user information.

  @param[in]  ptr1   Points to the first memory bytes to be compared.
  @param[in]  ptr2   Points to the second memory bytes to be compared.
  @param[in]  len    The number of bytes to be compared.

  @return
  This function returns 1 if the two buffers are different and
  0 if the two buffers are identical.

  @dependencies
  None.
*/

int timesafe_memcmp(const void* ptr1, const void* ptr2, size_t len);

#endif /* TLP11_UTILS_H */
