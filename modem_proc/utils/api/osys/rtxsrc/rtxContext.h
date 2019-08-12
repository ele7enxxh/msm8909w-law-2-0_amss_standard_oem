/*
 * Copyright (c) 2003-2013 Objective Systems, Inc.
 *
 * This software is furnished under a license and may be used and copied
 * only in accordance with the terms of such license and with the
 * inclusion of the above copyright notice. This software or any other
 * copies thereof may not be provided or otherwise made available to any
 * other person. No title to and ownership of the software is hereby
 * transferred.
 *
 * The information in this software is subject to change without notice
 * and should not be construed as a commitment by Objective Systems, Inc.
 *
 * PROPRIETARY NOTICE
 *
 * This software is an unpublished work subject to a confidentiality agreement
 * and is protected by copyright and trade secret law.  Unauthorized copying,
 * redistribution or other use of this work is prohibited.
 *
 * The above notice of copyright on this source code product does not indicate
 * any actual or intended publication of such source code.
 *
 *****************************************************************************/
/**
 * @file rtxContext.h
 * Common run-time context definitions.
 */
#ifndef _RTXCONTEXT_H_
#define _RTXCONTEXT_H_

#include "rtxsrc/rtxDList.h"

//#ifdef FEATURE_LTE
/* QC_MOD - needed to debug memory leak issues */
//#include "lte_cfg.h"
//#endif

/* QC_MOD - Added feature defs here */

#define _NO_LICENSE_CHECK
#define _COMPACT
#ifdef FEATURE_LTE_RRC_MEM_DEBUG
#define _DEBUG
#endif

//#define OSRTENCBUFSIZ 1024    /* dynamic encode buffer extent size     */
#define OSRTENCBUFSIZ 512

/**
 * @addtogroup rtxCtxt
 * @{
 */
/* run-time error info structures */

#define OSRTERRSTKSIZ	8	/* error stack size			*/
#define OSRTMAXERRPRM	5	/* maximum error parameters		*/

/**
 * @struct OSRTErrLocn rtxContext.h
 * Run-time error location structure
 *
 * This structure is a container structure that holds information on
 * the location within a C source file where a run-time error occurred.
 */
typedef struct {
   const OSUTF8CHAR* module;
   OSINT32	lineno;
} OSRTErrLocn;

/**
 * @struct OSRTErrInfo rtxContext.h
 * Run-time error information structure
 *
 * This structure is a container structure that holds information on
 * run-time errors.  The stack variable holds the trace stack information
 * that shows where the error occurred in the source code.  The parms
 * variable holds error parameters that are substituted into the message
 * that is returned to the user.
 */
typedef struct {
   OSRTErrLocn  stack[OSRTERRSTKSIZ];
   OSINT16      status;
   OSUINT8      stkx;
   OSUINT8      parmcnt;
   OSUTF8CHAR*  parms[OSRTMAXERRPRM];
   OSUTF8CHAR*  elemName;
} OSRTErrInfo;

typedef struct {
   OSRTDList list;              /* list of errors */
   OSRTErrInfo reserved;        /* error info elem, used if nomem to alloc */
   OSRTDListNode reservedNode;  /* node placeholder for errInfo elem       */
} OSRTErrInfoList;

/**
 * @struct OSRTBuffer rtxContext.h
 * Run-time message buffer structure
 *
 * This structure holds encoded message data.  For an encode operation,
 * it is where the message being built is stored.  For decode, it
 * holds a copy of the message that is being decoded.
 */
typedef struct {
   OSOCTET*     data;           /* pointer to start of data buffer      */
   OSSIZE       byteIndex;      /* byte index                           */
   OSSIZE       size;           /* current buffer size                  */
   OSINT16      bitOffset;      /* current bit offset (8 - 1)           */
   OSBOOL       dynamic;        /* is buffer dynamic?                   */
   OSBOOL       aligned;        /* is buffer byte aligned?              */
} OSRTBuffer;

typedef OSUINT32 OSRTFLAGS;

/**
 * @struct OSRTBufSave rtxContext.h
 * Structure to save the current message buffer state
 *
 * This structure is used to save the current state of the buffer.
 */
typedef struct {
   OSSIZE      byteIndex;      /* byte index                           */
   OSINT16     bitOffset;      /* current bit offset (8 - 1)           */
   OSRTFLAGS   flags;          /* flag bits                            */
} OSRTBufSave;

/* OSRTCTXT flag mask values : bits 32 - 16 are for common flags, bits  */
/* 15 - 0 are reserved for application specific flags                   */

#define OSDIAG          0x80000000  /* diagnostic tracing enabled       */
#define OSTRACE         0x40000000  /* tracing enabled                  */
#define OSDISSTRM       0x20000000  /* disable stream encode/decode     */
#define OSNOSTRMBACKOFF 0x8000000   /* stream mark/reset funcs is not used */
#define OS3GMOBORIG     0x4000000   /* 3G mobile-originated (net to MS) */

struct OSCTXT;
/**
 * OSRTFreeCtxtAppInfoPtr is a pointer to pctxt->pAppInfo free function,
 * The pctxt->pAppInfo (pXMLInfo and pASN1Info) should contain the pointer
 * to a structure and its first member should be a pointer to an appInfo
 * free function.
 */
typedef int (*OSFreeCtxtAppInfoPtr)(struct OSCTXT* pctxt);

/**
 * OSRTResetCtxtAppInfoPtr is a pointer to pctxt->pAppInfo reset function,
 * The pctxt->pAppInfo (pXMLInfo and pASN1Info) should contain the pointer
 * to a structure and its second member should be a pointer to appInfo reset
 * function.
 */
typedef int (*OSResetCtxtAppInfoPtr)(struct OSCTXT* pctxt);

/**
 * OSRTFreeCtxtGlobalPtr is a pointer to a memory free function.
 * This type describes the custom global memory free function generated by
 * the compiler to free global nmemory.  A pointer to a function of this
 * type may be stored in the context gblFreeFunc field in order to free
 * global data (pGlobalData) when rtxFreeContext is called.
 */
typedef void (*OSFreeCtxtGlobalPtr)(struct OSCTXT* pctxt);

/* Alias for __cdecl modifier; if __cdecl keyword is not supported,
 * redefine it as empty macro. */

#if !defined(OSCDECL)
#if defined(_MSC_VER) || defined(__BORLANDC__)
#define OSCDECL __cdecl
#else
#define OSCDECL
#endif
#endif /* OSCDECL */

/**
 * @struct OSCTXT rtxContext.h
 * Run-time context structure
 *
 * This structure is a container structure that holds all working
 * variables involved in encoding or decoding a message.
 */
typedef struct OSCTXT {          /* run-time context block               */
   void*         pMemHeap;       /* internal message memory heap         */
   OSRTBuffer    buffer;         /* data buffer                          */
   OSRTBufSave   savedInfo;      /* saved buffer info                    */
   OSRTErrInfoList errInfo;      /* run-time error info                  */
   OSUINT32      initCode;       /* code double word to indicate init    */
   OSRTFLAGS     flags;          /* flag bits                            */
   OSOCTET       level;          /* nesting level                        */
   OSOCTET       state;          /* encode/decode process state          */
   OSOCTET       diagLevel;      /* diagnostic trace level               */
   OSOCTET       lastChar;       /* last char/byte written or read       */
#ifndef _NO_STREAM
   struct OSRTSTREAM* pStream;   /* Stream                               */
   struct OSRTPrintStream *pPrintStrm; /* Print Stream                   */
#endif /* _NO_STREAM */
   OSRTDList elemNameStack;      /* element name stack                   */
   OSRTDList regExpCache;        /* compiled regular expression cache    */
   const OSOCTET* key;           /* pointer to run-time key data         */
   OSSIZE        keylen;         /* run-time key length                  */
   OSVoidPtr	 pXMLInfo;       /* XML specific info                    */
   OSVoidPtr	 pASN1Info;      /* ASN.1 specific info                  */
   OSVoidPtr     pUserData;      /* User defined data                    */
   OSVoidPtr     pGlobalData;    /* Global constant data                 */
   OSFreeCtxtGlobalPtr gblFreeFunc; /* Global free function              */
   OSVoidPtr     ssl;            /* SSL stack endpoint                   */
   struct OSRTDiagBitFieldList* pBitFldList; /* Bit field trace list     */
   OSUINT16      indent;         /* Print Stream indent                  */
   OSUINT16      version;        /* Protocol version                     */
} OSCTXT;

#define OSRT_GET_FIRST_ERROR_INFO(pctxt) \
(((pctxt)->errInfo.list.head == 0) ? (OSRTErrInfo*)0 : \
(OSRTErrInfo*)((pctxt)->errInfo.list.head->data))

#define OSRT_GET_LAST_ERROR_INFO(pctxt) \
(((pctxt)->errInfo.list.tail == 0) ? (OSRTErrInfo*)0 : \
(OSRTErrInfo*)((pctxt)->errInfo.list.tail->data))

/**
 * @}
 */
#ifndef _NO_STREAM

#define OSRTISSTREAM(pctxt) \
((pctxt)->pStream != 0 && !((pctxt)->flags & OSDISSTRM))

#else /* _NO_STREAM */

#define OSRTISSTREAM(pctxt) FALSE

#endif /* _NO_STREAM */

#define OSRTBUFCUR(pctxt)   (pctxt)->buffer.data[(pctxt)->buffer.byteIndex]
#define OSRTBUFPTR(pctxt)   &(pctxt)->buffer.data[(pctxt)->buffer.byteIndex]
#define OSRTBUFFER(pctxt)   (pctxt)->buffer.data
#define OSRTBUFSIZE(pctxt)  (pctxt)->buffer.size

#define OSRTBUFSAVE(pctxt) { \
(pctxt)->savedInfo.byteIndex = (pctxt)->buffer.byteIndex; \
(pctxt)->savedInfo.flags = (pctxt)->flags; }

#define OSRTBUFSAVE2(pctxt,pSavedBuf) { \
(pSavedBuf)->byteIndex = (pctxt)->buffer.byteIndex; \
(pSavedBuf)->bitOffset = (pctxt)->buffer.bitOffset; \
(pSavedBuf)->flags = (pctxt)->flags; }

#define OSRTBUFRESTORE(pctxt) { \
(pctxt)->buffer.byteIndex = (pctxt)->savedInfo.byteIndex; \
(pctxt)->flags = (pctxt)->savedInfo.flags; }

#define OSRTBUFRESTORE2(pctxt,pSavedBuf) { \
(pctxt)->buffer.byteIndex = (pSavedBuf)->byteIndex; \
(pctxt)->buffer.bitOffset = (pSavedBuf)->bitOffset; \
(pctxt)->flags = (pSavedBuf)->flags; }

#define OSRTBYTEALIGNED(pctxt) \
((pctxt)->buffer.bitOffset == 8 || (pctxt)->buffer.bitOffset == 0)

/* Pointers to C Run-Time memory allocation functions */

typedef void *(OSCDECL *OSMallocFunc ) (OSSIZE size);
typedef void *(OSCDECL *OSReallocFunc) (void *ptr, OSSIZE size);
typedef void  (OSCDECL *OSFreeFunc   ) (void *ptr);

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup rtxCtxt Context Management Functions
 * @{
 *
 * Context initialization functions handle the allocation, initialization, and
 * destruction of context variables (variables of type OSCTXT). These variables
 * hold all of the working data used during the process of encoding or decoding
 * a message. The context provides thread safe operation by isolating what
 * would otherwise be global variables within this structure. The context
 * variable is passed from function to function as a message is encoded or
 * decoded and maintains state information on the encoding or decoding process.
 */

/**
 * This function initializes an OSCTXT block. It sets all key working
 * parameters to their correct initial state values. It is required that this
 * function be invoked before using a context variable.
 *
 * @param pctxt        Pointer to the context structure variable to be
 *                       initialized.
 * @return             Completion status of operation:
 *                       - 0 = success,
 *                       - negative return value is error.
 */
#ifndef rtxInitContext
EXTERNRT int rtxInitContext (OSCTXT* pctxt);
#endif

/**
 * This function initializes an OSCTXT block. It sets all key working
 * parameters to their correct initial state values. It is required that this
 * function be invoked before using a context variable.
 *
 * @param pctxt        Pointer to the context structure variable to be
 *                       initialized.
 * @param malloc_func  Pointer to the memory allocation function.
 * @param realloc_func Pointer to the memory reallocation function.
 * @param free_func    Pointer to the memory deallocation function.
 * @return             Completion status of operation:
 *                       - 0 = success,
 *                       - negative return value is error.
 */
EXTERNRT int rtxInitContextExt (OSCTXT* pctxt,
                                OSMallocFunc malloc_func,
                                OSReallocFunc realloc_func,
                                OSFreeFunc free_func);

/**
 * This function initializes a context for use in a thread.  It is the
 * same as rtxInitContext except that it copies the pointer to constant
 * data from the given source context into the newly initialized thread
 * context.  It is assumed that the source context has been initialized
 * and the custom generated global initialization function has been
 * called.  The main purpose of this function is to prevent multiple
 * copies of global static data from being created within different
 * threads.
 *
 * @param pctxt        Pointer to the context structure variable to be
 *                       initialized.
 * @param pSrcCtxt     Pointer to source context which has been fully
 *                       initialized including a pointer to global
 *                       constant data initialized via a call to a
 *                       generated 'Init_&lt;project&gt;_Global' function.
 * @return             Completion status of operation:
 *                       - 0 = success,
 *                       - negative return value is error.
 */
EXTERNRT int rtxInitThreadContext (OSCTXT* pctxt, const OSCTXT* pSrcCtxt);

/**
 * This function assigns a message buffer to a context block. The block
 * should have been previously initialized by rtxInitContext.
 *
 * @param pctxt        The pointer to the context structure variable to be
 *                       initialized.
 * @param bufaddr      For encoding, the address of a memory buffer to receive
 *                       the encoded message. If this address is NULL (0),
 *                       encoding to a dynamic buffer will be done. For
 *                       decoding, the address of a buffer that contains the
 *                       message data to be decoded.
 * @param bufsiz       The size of the memory buffer. For encoding, this
 *                       argument may be set to zero to indicate a dynamic
 *                       memory buffer should be used.
 * @return             Completion status of operation:
 *                       - 0 = success,
 *                       - negative return value is error.
 */
EXTERNRT int rtxInitContextBuffer
(OSCTXT* pctxt, OSOCTET* bufaddr, OSSIZE bufsiz);

/**
 * This function is used to set the internal buffer pointer for
 * in-memory encoding or decoding.  It must be called after the
 * context variable is initialized before any other compiler generated
 * or run-time library encode function.
 *
 * @param pctxt         Pointer to a context structure.  This provides a
 *                      storage area for the function to store all working
 *                      variables that must be maintained between function
 *                      calls.
 *
 * @param bufaddr	A pointer to a memory buffer to use to encode a
 *                      message or that holds a message to be decoded.
 *                      The buffer should be declared as an array
 *                      of unsigned characters (OCTETs).  This parameter
 *                      can be set to NULL to specify dynamic encoding (i.e.,
 * 			the encode functions will dynamically allocate a
 *                      buffer to hold the encoded message).
 *
 * @param bufsiz	The length of the memory buffer in bytes.  Should
 *                      be set to zero if NULL was specified for bufaddr
 *                      (i.e. dynamic encoding was selected).
 */
EXTERNRT int rtxCtxtSetBufPtr
(OSCTXT* pctxt, OSOCTET* bufaddr, OSSIZE bufsiz);

/**
 * This macro returns the start address of an encoded message.
 * If a static buffer was used, this is simply the start address of
 * the buffer.  If dynamic encoding was done, this will return the
 * start address of the dynamic buffer allocated by the encoder.
 *
 * Note that this macro will not work with ASN.1 BER in-memory encoding.
 * In this case, the BER-specific version of the function must be used.
 *
 * @param pctxt         Pointer to a context structure.
 */
#define rtxCtxtGetMsgPtr(pctxt) (pctxt)->buffer.data

/**
 * This macro returns the length of an encoded message.  It will only
 * work for in-memory encoding, not for encode to stream.
 *
 * Note that this macro will not work with ASN.1 BER in-memory encoding.
 * In this case, the BER-specific version of the function must be used.
 *
 * @param pctxt         Pointer to a context structure.
 */
#define rtxCtxtGetMsgLen(pctxt) (pctxt)->buffer.byteIndex

/**
 * This function returns the total bit offset to the current element
 * in the context buffer.
 *
 * @param pctxt         Pointer to a context structure.
 * @return              Bit offset.
 */
EXTERNRT OSSIZE rtxCtxtGetBitOffset (OSCTXT* pctxt);

/**
 * This function sets the bit offset in the context to the given value.
 *
 * @param pctxt         Pointer to a context structure.
 * @param offset        Bit offset.
 * @return             Completion status of operation:
 *                       - 0 = success,
 *                       - Negative status code if error
 */
EXTERNRT int rtxCtxtSetBitOffset (OSCTXT* pctxt, OSSIZE offset);

/**
 * This function returns the count of bytes either written to a stream
 * or memory buffer.
 *
 * @param pctxt         Pointer to a context structure.
 * @return              I/O byte count.
 */
EXTERNRT OSSIZE rtxCtxtGetIOByteCount (OSCTXT* pctxt);

/**
 * This function verifies that the given context structure is
 * initialized and ready for use.
 *
 * @param pctxt        Pointer to a context structure.
 * @return             Completion status of operation:
 *                       - 0 = success,
 *                       - RTERR_NOTINIT status code if not initialized
 */
EXTERNRT int rtxCheckContext (OSCTXT* pctxt);

/**
 * This function frees all dynamic memory associated with a context. This
 * includes all memory allocated using the rtxMem functions using the given
 * context parameter.
 *
 * @param pctxt        Pointer to a context structure.
 */
EXTERNRT void rtxFreeContext (OSCTXT* pctxt);

/**
 * This function creates a copy of a context structure. The copy is a "shallow
 * copy" (i.e. new copies of dynamic memory blocks held within the context are
 * not made, only the pointers are transferred to the new context structure).
 * This function is mainly for use from within compiler-generated code.
 *
 * @param pdest        - Context structure to which data is to be copied.
 * @param psrc         - Context structure from which data is to be copied.
 */
EXTERNRT void rtxCopyContext (OSCTXT* pdest, OSCTXT* psrc);

/**
 * This function is used to set a processing flag within the context structure.
 *
 * @param pctxt        - A pointer to a context structure.
 * @param mask         - Mask containing bit(s) to be set.
 */
EXTERNRT void rtxCtxtSetFlag (OSCTXT* pctxt, OSUINT32 mask);

/**
 * This function is used to clear a processing flag within the context
 * structure.
 *
 * @param pctxt        - A pointer to a context structure.
 * @param mask         - Mask containing bit(s) to be cleared.
 */
EXTERNRT void rtxCtxtClearFlag (OSCTXT* pctxt, OSUINT32 mask);

/**
 * This macro tests if the given bit flag is set in the context.
 *
 * @param pctxt        - A pointer to a context structure.
 * @param mask         - Bit flag to be tested
 */
#define rtxCtxtTestFlag(pctxt,mask) (((pctxt)->flags & mask) != 0)

/**
 * This function is used to push an array element name onto the context
 * element name stack.  The name is formed by combining the given element
 * name with the index to create a name of format name[index].
 * Dynamic memory is allocated for the resulting name using the
 * rtxMemAlloc function.
 *
 * @param pctxt        Pointer to a context structure.
 * @param elemName     Name of element to be pushed on stack.
 * @param idx          Index or the array element.
 * @return             Completion status of operation:
 *                       - 0 = success,
 *                       - RTERR_NOMEM if mem alloc for name fails.
 */
EXTERNRT int rtxCtxtPushArrayElemName
(OSCTXT* pctxt, const OSUTF8CHAR* elemName, OSSIZE idx);

/**
 * This function is used to push an element name onto the context
 * element name stack.
 *
 * @param pctxt        Pointer to a context structure.
 * @param elemName     Name of element to be pushed on stack.  Note that
 *                       a copy of the name is not made, the pointer to
 *                       the name that is passed is stored.
 * @return             Completion status of operation:
 *                       - 0 = success,
 *                       - RTERR_NOMEM if mem alloc for name fails.
 */
EXTERNRT int rtxCtxtPushElemName (OSCTXT* pctxt, const OSUTF8CHAR* elemName);

/**
 * This function is used to push a type name onto the context
 * element name stack.  The name is only added for the top-level type.
 * This is determined by testing to ensure that there are no existing
 * names on the stack.
 *
 * @param pctxt        Pointer to a context structure.
 * @param typeName     Name of type to be pushed on stack.  Note that
 *                       a copy of the name is not made, the pointer to
 *                       the name that is passed is stored.
 * @return             Completion status of operation:
 *                       - 0 = success,
 *                       - RTERR_NOMEM if mem alloc for name fails.
 */
EXTERNRT int rtxCtxtPushTypeName (OSCTXT* pctxt, const OSUTF8CHAR* typeName);

/**
 * This function pops the last element name from the context stack.
 * This name is assumed to be an array element name pushed by the
 * rtxCtxtPushArrayElemName function.  The name is therefore dynamic
 * and memory is freed for it using the rtxMemFreePtr function.
 *
 * @param pctxt        Pointer to a context structure.
 * @return             True if name popped from stack or false if stack
 *                       is empty.
 */
EXTERNRT OSBOOL rtxCtxtPopArrayElemName (OSCTXT* pctxt);

/**
 * This function pops the last element name from the context stack.
 *
 * @param pctxt        Pointer to a context structure.
 * @return             Element name popped from stack or NULL if stack
 *                       is empty.
 */
EXTERNRT const OSUTF8CHAR* rtxCtxtPopElemName (OSCTXT* pctxt);

/**
 * This function pops the type name from the context stack.  The name is
 * only popped if the item count is one.
 *
 * @param pctxt        Pointer to a context structure.
 * @return             Type name popped from stack or NULL if stack
 *                       count not equal to one.
 */
EXTERNRT const OSUTF8CHAR* rtxCtxtPopTypeName (OSCTXT* pctxt);

/**
 * This macro returns the last element name from the context stack.
 *
 * @param pctxt        Pointer to a context structure.
 * @return             Element name from top of stack or NULL if stack
 *                       is empty.
 */
#define rtxCtxtPeekElemName(pctxt) \
(((pctxt)->elemNameStack.count > 0) ? \
 (const OSUTF8CHAR*)(pctxt)->elemNameStack.tail->data : (const OSUTF8CHAR*)0)

EXTERNRT int rtxPreInitContext (OSCTXT* pctxt);

/**
 * This function sets flags to a heap. May be used to control the heap's
 * behavior.
 *
 * @param pctxt        Pointer to a memory block structure that contains the
 *                       list of dynamic memory block maintained by these
 *                       functions.
 * @param flags        The flags.
 */
EXTERNRT void rtxMemHeapSetFlags (OSCTXT* pctxt, OSUINT32 flags);

/**
 * This function clears memory heap flags.
 *
 * @param pctxt        Pointer to a memory block structure that contains the
 *                       list of dynamic memory block maintained by these
 *                       functions.
 * @param flags        The flags
 */
EXTERNRT void  rtxMemHeapClearFlags (OSCTXT* pctxt, OSUINT32 flags);

/**
 * This macro will byte-align the context buffer.
 */
#define rtxByteAlign(pctxt) \
if ((pctxt)->buffer.bitOffset != 8) { \
(pctxt)->buffer.byteIndex++; (pctxt)->buffer.bitOffset = 8; }

/**
 * This function saves the current position in a message buffer or stream.
 *
 * @param pctxt        Pointer to a context block.
 * @param ppos         Pointer to saved position.
 * @return             Completion status of operation:
 *                       - 0 = success,
 *                       - negative return value is error.
 */
EXTERNRT int rtxMarkPos (OSCTXT* pctxt, OSSIZE* ppos);

/**
 * This function resets a message buffer or stream back to the given position.
 *
 * @param pctxt        Pointer to a context block.
 * @param pos          Context position.
 * @return             Completion status of operation:
 *                       - 0 = success,
 *                       - negative return value is error.
 */
EXTERNRT int rtxResetToPos (OSCTXT* pctxt, OSSIZE pos);

/**
 * This macro sets the protocol version in the context.  This version
 * number may be used in application code to do version specific operations.
 * It is used in generated ASN.1 code with the extension addition version
 * numbers to determine if an addition should be decoded.
 *
 * For example, if this value is set to 8 and an extension addition group
 * exists with version number 9 ([[ 9: ... ]]), its contents will not be
 * decoded.
 *
 * @param pctxt        Pointer to a context structure.
 * @return             Element name from top of stack or NULL if stack
 *                       is empty.
 */
#define rtxCtxtSetProtocolVersion(pctxt,value) (pctxt)->version = value

#define rtxMarkBitPos(pctxt,ppos,pbitoff) \
(*(pbitoff) = (OSUINT8) (pctxt)->buffer.bitOffset, rtxMarkPos (pctxt, ppos))

#define rtxResetToBitPos(pctxt,pos,bitoff) \
((pctxt)->buffer.bitOffset = (OSUINT8) bitoff, rtxResetToPos (pctxt, pos))

#ifndef _COMPACT
#define RTXCTXTPUSHARRAYELEMNAME(pctxt,name,idx)   \
rtxCtxtPushArrayElemName(pctxt,OSUTF8(name),idx)

#define RTXCTXTPOPARRAYELEMNAME(pctxt) \
rtxCtxtPopArrayElemName(pctxt)

#define RTXCTXTPUSHELEMNAME(pctxt,name) \
rtxCtxtPushElemName(pctxt,OSUTF8(name))

#define RTXCTXTPOPELEMNAME(pctxt) \
rtxCtxtPopElemName(pctxt)

#define RTXCTXTPUSHTYPENAME(pctxt,name) \
rtxCtxtPushTypeName(pctxt,OSUTF8(name))

#define RTXCTXTPOPTYPENAME(pctxt) \
rtxCtxtPopTypeName(pctxt)
#else
#define RTXCTXTPUSHARRAYELEMNAME(pctxt,name,idx)
#define RTXCTXTPOPARRAYELEMNAME(pctxt)
#define RTXCTXTPUSHELEMNAME(pctxt,name)
#define RTXCTXTPOPELEMNAME(pctxt)
#define RTXCTXTPUSHTYPENAME(pctxt,name)
#define RTXCTXTPOPTYPENAME(pctxt)
#endif

#ifdef __cplusplus
}
#endif
/**
 * @} rtxCtxt
*/
#endif
