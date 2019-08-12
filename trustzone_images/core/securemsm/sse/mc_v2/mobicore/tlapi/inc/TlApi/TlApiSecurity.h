/** @addtogroup TLAPI_SEC
 * @{
 *
 * The secure object API provides integrity, confidentiality and authenticity
 * for data that is sensitive but needs to be stored in untrusted (normal world)
 * memory. Secure objects provide device binding. Respective objects are only
 * valid on a specific device.
 *
 * There are two core operations of this API:
 *
 * - wrap() which encloses user data in a secure object.
 * - unwrap() which extracts user data from a secure object.
 *
 * The userdata is diveided int data that will remain as plain data and
 * data that will be encrypted:
 *
 *  <pre>
 *  <code>
 *
 *           +------------------+------------------------+
 *           |   plain-data     |  to-be-encrypted-data  |
 *           +------------------+------------------------+
 *
 *           /-------------- user data ------------------/
 *           /- data that is    /-- data that will be
 *              not going to        encrypted. This data
 *              be encrypted        is encrypted by the
 *              and will remain     wrapper function ----/
 *              as cleartext ---/
 *
 *
 *           /---- plainLen ----/------ encryptLen ------/
 *
 *
 *  </code>
 *  </pre>
 *
 * A secure object looks like this:
 *
 *  <pre>
 *  <code>
 *
 *  +--------+---------------------+-------------------+--------+---------+
 *  | Header |      plain-data     |   encrypted-data  |  hash  | random  |
 *  +--------+---------------------+-------------------+--------+---------+
 *
 *
 *  /--------/---- plainLen -------/-- encryptedLen ---/-- 32 --/- 1..16 -/
 *
 *  /----------------- toBeHashedLen  -----------------/
 *
 *                                 /---------- toBeEncryptedLen ----------/
 *
 *  </code>
 *  </pre>
 *
 * DATA INTEGRITY:
 *
 * A secure object contains a message digest (hash, random) that ensures data
 * integrity of the user data. The hash value is computed and stored during the
 * wrap() operation as well as adding a random number (before data encryption
 * takes place) and recomputed and compared during the unwrap() operation
 * (after the data has been decrypted).
 *
 * CONFIDENTIALITY:
 *
 * Secure objects are encrypted using context-specific keys that are never
 * exposed, neither to the normal world, nor to the Trustlet. It is up to the
 * user to define how many bytes of the user data are to be kept in plain text
 * and how many bytes are to be encrypted. The plain text part of a secure
 * object always precedes the encrypted part.
 *
 * AUTHENTICITY:
 *
 * As a means of ensuring the trusted origin of secure objects, the wrap
 * operation stores the Trustlet id (SPID, UUID) of the calling Trustlet in
 * the secure object header (as producer). This allows Trustlets to only accept
 * secure objects from certain partners. This is most important for scenarios
 * involving secure object sharing.
 *
 * CONTEXT:
 *
 * The concept of context allows for sharing of secure objects. At present there
 * are three kinds of context:
 *
 * - MC_SO_CONTEXT_TLT: Trustlet context. The secure object is confined to a
 *   particular Trustlet. This is the standard use case.
 *         - PRIVATE WRAPPING: If no consumer was specified, only the Trustlet
 *           that wrapped the secure object can unwrap it.
 *         - DELEGATED WRAPPING: If a consumer Trustlet is specified, only the
 *           Trustlet specified as 'consumer' during the wrap operation can
 *           unwrap the secure object.
 *           Note that there is no delegated wrapping with any other contexts.
 *
 * - MC_SO_CONTEXT_SP: Service provider context. Only Trustlets that belong to
 *   the same service provider can unwrap a secure object that was wrapped in
 *   the context of a certain service provider.
 *
 * - MC_SO_CONTEXT_DEVICE: Device context. All Trustlets can unwrap secure
 *   objects wrapped for this context.
 *
 * Default flag TLAPI_UNWRAP_DEFAULT permits only trustlet context and
 * no delegation. Include flag TLAPI_UNWRAP_PERMIT_DELEGATED if you want to allow delegated objects.
 * Include flags TLAPI_UNWRAP_PERMIT_CONTEXT_SP or TLAPI_UNWRAP_PERMIT_CONTEXT_DEVICE if you want to
 * permit unwrapping with those context.


 * LIFETIME:
 *
 * The concept of a lifetime allows to limit how long a secure object is valid.
 * After the end of the lifetime, it is impossible to unwrap the object. At
 * present, three lifetimes are defined:
 *
 * - MC_SO_LIFETIME_PERMANENT: Secure Object does not expire.
 *
 * - MC_SO_LIFETIME_POWERCYCLE: Secure Object expires on reboot.
 *
 * - MC_SO_LIFETIME_SESSION: Secure Object expires when Trustlet session is closed.
 *   The secure object is thus confined to a particular session of a particular
 *   Trustlet.
 *   Note that session lifetime is only allowed for private wrapping in the Trustlet
 *   context MC_SO_CONTEXT_TLT.
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

#ifndef __TLAPISECURITY_H__
#define __TLAPISECURITY_H__

#include "mcSo.h"

#include "TlApi/TlApiCommon.h"
#include "TlApi/TlApiError.h"

/** Wraps user data given in source buffer and creates a secure object in the
 * destination buffer.
 *
 * The required size of the destination buffer (total size of secure object) can
 * be obtained through the MC_SO_SIZE() macro. The input to this macro is the
 * length of the plain data and the the length of the data that is to be encrypted.
 *
 * Example:
 *
 *  <pre>
 *
 *  secureObjectLength = MC_SO_SIZE(plainLength, encryptLength)
 *
 *  </pre>
 *
 * Since dynamic memory allocations are not supported in the Secure World,
 * i.e. the Trustlets must allocate memory statically, the MC_SO_SIZE() macro
 * can be used for statically allocating memory with the size of the
 * Secure Object. Example:
 *
 *  <pre>
 *
 *  uint8_t dataBuf[MC_SO_SIZE(plainLength, encryptLength)];
 *
 *
 * Source and destination addresses may overlap, thus the following code is a
 * typical usage pattern:
 *
 *  <pre>
 *
 *  union {
 *      uint8_t src[100];
 *      uint8_t dst[MC_SO_SIZE(30, 70)]; // 30 bytes plain, 70 bytes encrypted.
 *  } buffer;
 *
 * </pre>
 *
 *  size_t soLen = sizeof(buffer);
 *
 *  // Fill source.
 *  buffer.src[0] = 'H';
 *  ...
 *
 *  if (TLAPI_OK == tlApiWrapObject(
 *      buffer.src, 30, 70, buffer.dst, &soLen, MC_SO_CONTEXT_TLT, NULL)) {
 *      ...
 *  }
 *
 * </pre>
 *
 * @param [in] src User data. The data which that is created by the user. The user data is divided
 * into two types i.e. data that will remain cleartext and will not be encrypted and the data
 * that will be encrypted into the secure object.
 * Note! It can be a good programming exercise/experiment to check the Secure Object data and
 * there find out that some part of the SO is plain-text and therefore readable.
 * @param [in] plainLen Length of plain text user data (from beginning of src). This is the length of the
 * userdata that is going to remain as plain text (plain data), i.e. not be encrypted.
 * @param [in] encryptedLen Length of to-be-encrypted user data (after plain text
 * user data). This is the Length of the data that is going to be encrypted. The offset is after
 * the last byte of the plain text.
 * @param [in, out] dest Destination buffer (secure object). Every secure object starts with the header,
 * so pass the header into this function. Note that the pointer must be word-aligned.
 * @param [in, out] destLen [in] Length of the destination buffer. [out] Length
 * of the secure object.
 * @param [in] context Key context.
 *
 * @param  [in] lifetime Expiry type of secure object.
 *  - MC_SO_LIFETIME_PERMANENT: Secure Object does not expire.
 *  - MC_SO_LIFETIME_POWERCYCLE: Secure Object expires on reboot.
 *  - MC_SO_LIFETIME_SESSION: Secure Object expires when Trustlet session is closed.
 *
 * @param  [in] consumer NULL or Trustlet/service provider identifier for delegated wrapping.
 * Delegated wrapping makes it possible for other Trustlets to unwrap the secure object.
 * Such scenario can be communication between trustlets. It can be a service provider
 * that is using several trustlets which are communicating with each other.
 *
 * @param [in] flags Use the TLAPI_WRAP_DEFAULT flag
 *
 *
 * The return codes:
 *
 * @return TLAPI_OK if operation was successful.
 * @retval E_TLAPI_NULL_POINTER If a pointer input parameter is NULL.
 * @retval E_TLAPI_INVALID_INPUT If an input parameter is invalid, for instance
 * if the maximum payload size is exceeded.
 * @retval E_TLAPI_CR_WRONG_OUTPUT_SIZE If the output buffer is too small.
 * @retval E_TLAPI_UNALIGNED_POINTER    If the secure object pointer is not word-aligned.
 * @retvar E_TLAPI_UNMAPPED_BUFFER      If one buffer is not entirely mapped in Trustlet address space.
 */
_TLAPI_EXTERN_C tlApiResult_t tlApiWrapObjectExt(
    const void *src,
    size_t plainLen,
    size_t encryptedLen,
    void *dest,
    size_t *destLen,
    mcSoContext_t context,
    mcSoLifeTime_t lifetime,
    const tlApiSpTrustletId_t *consumer,
    uint32_t flags);


#define TLAPI_WRAP_DEFAULT 0
#define TLAPI_WRAP_FORMAT21 2

#define TLAPI_WRAP_DELEGATE_WITH_ROOTID 4

#define TLAPI_WRAP_COMPATIBILITY_BIT 1

/* Compatibility macro, avoid using this */
#define TLAPI_WRAP_COMPATIBILITY (TLAPI_WRAP_COMPATIBILITY_BIT|TLAPI_WRAP_FORMAT21)

static inline tlApiResult_t tlApiWrapObject(
    const void *src,
    size_t plainLen,
    size_t encryptedLen,
    void *dest,
    size_t *destLen,
    mcSoContext_t context,
    mcSoLifeTime_t lifetime,
    const tlApiSpTrustletId_t *consumer,
    uint32_t flags)
{
    return tlApiWrapObjectExt(
        src,
        plainLen,
        encryptedLen,
        dest,
        destLen,
        context,
        lifetime,
        consumer,
        flags);
}

/** Unwraps a secure object.
 *
 * Decrypts and verifies checksum of given object for the context indicated in
 * the secure object's header.
 *
 * Verifies and decrypts a secure object and stores the user data (plain data and the
 * decrypted data) to a given location. For further details refer to tlApiWrapObject().
 *
 * After this operation, the source address contains the decrypted secure
 * object (whose user data starts immediately after the secure object header),
 * or the attempt of the decryption, which might be garbage, in case the
 * decryption failed (due to a wrong context, for instance).
 *
 * If dest is not NULL, copies the decrypted user data part to the specified
 * location, which may overlap with the memory occupied by the original secure
 * object.
 *
 * @param [in,out] src [in] Encrypted secure object, [out] decrypted secure
 * object i.e. the secure object header data the plain data and the decrypted
 * data (which was earlier encrypted by the wrapper function).
 * @param [in] srcLen Length of source buffer i.e. the length of the secure object.
 * @param [in,out] dest Address of user data or NULL if no extraction of user data
 * is desired. Note that this buffer has to be statically allocated (which is the reason
 * why it also is set as input parameter). The tlApiWrapObjectExt does not allocate the
 * buffer, it only writes to the buffer from the starting address and maximum of
 * destLen (see parameter below).
 * @param [in,out] destLen [in] Length of destination buffer. [out] Length of
 * user data. The length of the statically allocated buffer is sent as input for
 * copying the userdata after the decryption of the secure object. The length of
 * the userdata is returned.
 * @param [in] flags See more explanation at the top, in the CONTEXT part.
 *
 *
 * The return values of the unwraapper function:
 * @return TLAPI_OK if operation was successful.
 * @retval E_TLAPI_INVALID_INPUT If an input parameter is invalid.
 * @retval E_TLAPI_CR_WRONG_OUTPUT_SIZE If the output buffer is too small.
 * @retval E_TLAPI_SO_WRONG_VERSION If the version of the secure object is not
 * supported.
 * @retval E_TLAPI_SO_WRONG_TYPE If the kind of secure object is not supported.
 * @retval E_TLAPI_SO_WRONG_LIFETIME If the kind of lifetime of the secure
 * object is not supported.
 * @retval E_TLAPI_SO_WRONG_CONTEXT If the kind of context of the secure object
 * is not supported.
 * @retval E_TLAPI_SO_WRONG_CHECKSUM If (after decryption) the checksum over the
 * whole secure object (header and payload) is wrong. This is usually an
 * indication that the secure object has been tampered with, or that the client
 * calling unwrap is not allowed to unwrap the secure object.
 * @retval E_TLAPI_UNMAPPED_BUFFER  If one buffer is not entirely mapped in Trustlet address space.
 * @retval E_TLAPI_SO_DELEGATED_NOT_PERMITTED Delegated objects were not permitted.
 * @retval E_TLAPI_SO_CONTEXT_NOT_PERMITTED This context was not permitted.
 */
_TLAPI_EXTERN_C tlApiResult_t tlApiUnwrapObjectExt(
    void *src,
    size_t srcLen,
    void *dest,
    size_t *destLen,
    uint32_t flags );


#define TLAPI_UNWRAP_COMPATIBILITY_BIT     0x0001
#define TLAPI_UNWRAP_PERMIT_DELEGATED      0x0002
#define TLAPI_UNWRAP_PERMIT_CONTEXT_TL     0x0004
#define TLAPI_UNWRAP_PERMIT_CONTEXT_SP     0x0008
#define TLAPI_UNWRAP_PERMIT_CONTEXT_DEVICE 0x0010

#define TLAPI_UNWRAP_PERMIT_CONTEXT_MASK   0x001C

#define TLAPI_UNWRAP_DEFAULT TLAPI_UNWRAP_PERMIT_CONTEXT_TL


static inline tlApiResult_t tlApiUnwrapObject(
    void *src,
    size_t srcLen,
    void *dest,
    size_t *destLen,
    uint32_t flags )
{
    return tlApiUnwrapObjectExt(
        src,
        srcLen,
        dest,
        destLen,
        flags );
}



/** Derive key
 *
 * Derive key specific to trustlet, SP or device (depending on context parameter).
 * Derived key is valid permanently, during current powercycle or during current
 * session (depending on lifetime parameter).
 *
 * Different salt values provide statistically different key.
 * Resulting key is expanded to destLen bytes using RFC5869 expansion.
 *
 * @param [in] salt [in] Salt value for key derivation.
 * @param [in] saltLen [in] Length of salt value.
 * @param [out] dest [out] Resulsting key.
 * @param [out] destLen [out] Length of desired key.
 * @param [in] context [in] Content for derived key.
 * @param [in] lifetime [in] Lifetime for derived key.
 *
 * @return TLAPI_OK if operation was successful.
 */
_TLAPI_EXTERN_C tlApiResult_t tlApiDeriveKey(
    const void *salt,
    size_t saltLen,
    void *dest,
    size_t destLen,
    mcSoContext_t context,
    mcSoLifeTime_t lifetime
);

//------------------------------------------------------------------------------
/** Real time sources in MobiCore */
typedef enum {
        TS_SOURCE_ILLEGAL       = 0,    /**< Illegal counter source value. */
        TS_SOURCE_SOFTCNT       = 1,    /**< monotonic counter that is reset upon power cycle. */
        TS_SOURCE_SECURE_RTC    = 2,    /**< Secure real time clock that uses underlying hardware clock. */
} tsSource_t;

/** Time stamp value*/
typedef uint64_t timestamp_t;

///** Retrieve a time stamp value.
// *
// * Returns a time stamp value. The time stamp value is granted to be monotonic depending on the
// * time stamp source it is based on. The implementation takes care that no time stamp overflow occurs.
// *
// * Two subsequent calls to the function return always two different time stamp values where the
// * second value is always bigger than the first one. If the first call to the function returns
// * val1 as time stamp value, second call returns val2. For the returned values the following rules apply:
// *
// * val2 >= (val1 + 1)
// *
// * When a time stamps source is not supported by the platform. The function returns E_TLAPI_NOT_IMPLEMENTED.
// *
// * @param ts [out] After successful execution the time stamp is returned in the reference variable.
// * @param source [in] Time stamp source
// *
// * @returns TLAPI_OK if operation was successful.
// * @returns E_TLAPI_NOT_IMPLEMENTED if source is not supported by the platform.
// * @returns E_TLAPI_UNMAPPED_BUFFER if one buffer is not entirely mapped in Trustlet address space.
// */
//_TLAPI_EXTERN_C tlApiResult_t tlApiGetTimeStamp(
//    timestamp_t *ts,
//    tsSource_t  source
//);

#endif // __TLAPISECURITY_H__

/** @} */
