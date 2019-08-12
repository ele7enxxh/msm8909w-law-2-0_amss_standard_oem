#ifndef COMDEF_H
#define COMDEF_H
/**
  @file comdef.h
  @brief This header file contains general types and macros that are of use
  to all modules.  

*/
/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The Utility_Services_API_mainpage.dox file contains all file/group 
      descriptions that are in the output PDF generated using Doxygen and 
      Latex. To edit or update any of the file/group text in the PDF, edit 
      the Utility_Services_API_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the "utils_services" 
      group description in the Utility_Services_API_mainpage.dox file. 
===========================================================================*/
/*===========================================================================

                   S T A N D A R D    D E C L A R A T I O N S

DESCRIPTION
  This header file contains general types and macros that are of use
  to all modules.  The values or definitions are dependent on the specified
  target.  T_WINNT specifies Windows NT based targets, otherwise the
  default is for ARM targets.

       T_WINNT  Software is hosted on an NT platforn, triggers macro and
                type definitions, unlike definition above which triggers
                actual OS calls

DEFINED CONSTANTS

       Name      Definition
       -------   --------------------------------------------------------
       ON        Asserted condition
       OFF       Deasserted condition

       NULL      Pointer to nothing

       PACK()    Used to declare a C type packed for either GCC or ARM
                 compilers.

       *** DEPRECATED - WON'T WORK FOR newer versions (3.x) of GCC ***
       PACKED    Used to indicate structures which should use packed
                 alignment
       *** DEPRECATED - WON'T WORK FOR newer versions (3.x) of GCC ***

       INLINE    Used to inline functions for compilers which support this

Copyright (c) 1992-2011 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary. 
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/api/services/comdef.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/22/11   llg     (Tech Pubs) Edited/added Doxygen comments and markup.
11/29/10   EBR     Doxygenated File
02/20/08   tjc     Fix ALIGN macro for GCC targets
                   Add FEATURE_NO_INTLOCK and FEATURE_NO_TASKLOCK
02/05/08   ejv     Sync with version from MSMSHARED_UTILS.00.00.11.
                   Move BM macro to sys.h.
12/11/07   pj      #defined INTLOCK()/INTFREE() to no-op for WINCE_OAL.
06/05/07   trc     Added HANDPACK() macro allowing for explicit hand-packing
05/08/07   trc     Added STATIC pseudo-keyword, make "" by defining NO_STATIC
04/20/07   trc     Added PACK() macro for true ARM/GCC packing portability
3/12/07    RC      Added EXTERM, BM() and DEC_TO_0() macros.
04/02/06   rp      Featurized the define for boolean since 32bit mode boot code
                   needs all variable to be 32bit wide
03/15/06   tmz     Merge changes from 6800
08/25/04   jac     Suppress lint error 413 for FPOS macro
08/23/04    gr     Added !defined _ARM_ASM_ protection for function
                   prototypes.
03/22/04    gr     Undid the unconditional defining away of STATIC. This is not
                   needed.
11/19/03    gr     Added multiple definition protection for the ARR_SIZE
                   macro.
10/18/03    gr     Moved TASKLOCK/TASKFREE into this file from rex.h. DMSS
                   expects to pick these up just by including comdef.h.
10/17/03    gr     Made inclusion of target.h unconditional (TG is now
                   deprecated).
10/16/03    gr     Defined LOCAL away unconditionally. DMSS doesn't build with
                   LOCAL set to static.
01/07/04   ajn     Re-integrated 11/14/03 change
11/20/03   ak      ARR_SIZE conflict with BREW files fixed
11/14/03   ajn     Removed #include "rex.h" to avoid circular include issue.
                   Instead, prototypes for rex_int_lock()/rex_int_free()
                   have been explicitly included for INTLOCK/INTFREE macros.
10/01/03    gr     Deleted misleading INTLOCK comments.
06/12/02   ems     Added SWIG preprocessor work around
06/11/01    gr     Added MOD_BY_POWER_OF_TWO macro.
04/12/01    sq     Added inpdw and outpdw macros
           vas     Added solaris support
04/06/01    gr     Removed the definitions of abs and labs.
01/25/01   day     Merged from MSM5105_COMMON.00.00.05.
                   Added include for stdlib.h within WINNT ifdef
                   Added support for ARM_ASM
                   Added support SWIG preprocessor.
09/09/99   tac     Merged in a bunch of panther changes.
08/11/99   jct     Corrected conditional compilation around labs to check for
                   labs instead of abs
06/30/99   jct     Added PC_EMULATOR capability for INTLOCK and INTLOCKSAV
06/15/99   jct     Cleanup to make work in Windows NT environment.  Addition
                   of int(n) types where n is 8,16,32,64 and addition of
                   unit64.  Removal of OS comments and defines as this is replaced
                   by T_WINNT.  Conditional inclusion of MAX, MIN, labs, abs,
                   NULL definitions.  Removal of volatile from packed definition
                   stdlib.h included for Win32 based targets since this replaces several
                   of the define here.  Changed out uts calls to be comet calls.
                   Added a defines for Windows NT targets to define away symbols
                   no longer supported
04/09/99    ms     Lint cleanup.
04/01/99    ms     Lint cleanup.
02/17/99    ms     Parenthesized abs and labs.
                   Defined __packed to volatile to exploit lint.
12/16/98   jct     Removed 80186 support
12/01/98    ms     Removed definition of bsp_io_base.
11/20/98    ms     Typecast to volatile pointers in inp,outp,inpw and outpw.
10/10/98   jct     Honing of OS and T_ definitions, added T_WINNT as a target
                   which triggers something different then which OS since
                   we may be running REX on an NT platform, but strictly
                   speaking, the application software is using REX, not NT
                   as the OS.  Changed ROM for WINNT/ARM targets to be nothing
                   instead of const
09/09/98   jct     Updates to merge ARM support with 186 support, added some
                   new type aliases, added some segmented architecture macros,
                   updated comments, removed unused code, updated declaration
                   of inp/outp to have cdecl
08/01/98   jct     Updates to support ARM processor
03/25/96   jah     Moved SET_VECT to 80186.h
02/06/96   fkm     Put ()s around Macros to Make Lint Happy
01/12/95   jah     Updated dependency in MACRO INTLOCK_SAV / INTFREE_SAV.
12/20/94   jah     Corrected typo in ARR_SIZE() macro comments
03/11/94   jah     Added #ifdef _lint of NULL as 0 for near/far lint problem.
06/11/93   jah     Changed FPOS() to cast the pointer to 'near' to avoid the
                   compiler complaints about lost segments.
01/28/93   twp     Added ARR_SIZE macro to return number of array elements.
07/17/92   jah     Changed int1 from 'char' to 'signed char'
06/10/92   jah     Added WORD_LO/HI, INC_SAT
05/19/92   jah     Added header comments for macros & made them lint-friendly
05/04/92   jah     Added define for ROM
03/02/92   jah     Added in/outp prototypes, INT* Macro comments w/lint
02/20/92   arh     Added LOCAL macro and ifdef/endif around file

===========================================================================*/


/*===========================================================================

                            Data Declarations

===========================================================================*/

#include "coreapi_variation.h"
#ifdef __cplusplus
extern "C" {
#endif      
#include "com_dtypes.h"

/* For NT apps we want to use the Win32 definitions and/or those
** supplied by the Win32 compiler for things like NULL, MAX, MIN
** abs, labs, etc.
*/
#ifdef T_WINNT
   #error code not present
#endif

/* Macro to indicate that a variable is not being used to suppress lint
** warnings
*/
#define NOTUSED(i) if(i){}

#define WCE_MSG  RETAILMSG
#define WCE_MSG6 RETAILMSG
#define WCE_MSGT RETAILMSG
#define WCE_MSG_SDBG RETAILMSG

#if defined(FEATURE_QUBE)
/* QDSP6 tools release after Dec. 12 no longer define addr_t in qtypes.h */
typedef  void *            addr_t;
#endif /* !QDSP6_LEGACY_ADDR_T */

/* ---------------------------------------------------------------------
** Compiler Keyword Macros
** --------------------------------------------------------------------- */

/** @addtogroup utils_services
@{ */

#if (! defined T_WINNT) && (! defined __GNUC__)
  
    /* Non WinNT Targets */

    #if defined(__ARMCC_VERSION) 
      #define PACKED __packed
      #define PACKED_POST
      #define ALIGN(__value) __align(__value)
      #define POST_ALIGN(__value)
      #ifndef INLINE
      #define INLINE __inline
      #endif 
      #define inline __inline
      #define PACKED_UINT16_ACCESS(PTR) *((__packed uint16 *) (PTR))
      #define PACKED_UINT16 __packed uint16
      #define PACKED_UINT32_ACCESS(PTR) *((__packed uint32 *) (PTR))
      #define PACKED_UINT32 __packed uint32
      #define PACKED_UINT64_ACCESS(PTR) *((__packed uint64 *) (PTR))
      #define PACKED_UINT64 __packed uint64
      #define PACKED_INT16_ACCESS(PTR) *((__packed int16 *) (PTR))
      #define PACKED_INT16 __packed int16
      #define PACKED_INT32_ACCESS(PTR) *((__packed int32 *) (PTR))
      #define PACKED_INT32 __packed uint32
      #define PACKED_INT64_ACCESS(PTR) *((__packed int64 *) (PTR))
      #define PACKED_INT64 __packed int64
    #else  /* __GNUC__ */
      /** @cond
      */
      #define PACKED 
      /** @endcond */
      #ifndef FEATURE_WINCE
        /** @cond
        */
        #define ALIGN(__value) __attribute__((__aligned__(__value)))
        #define POST_ALIGN(__value)
        #define PACKED_POST    __attribute__((__packed__))
        /** @endcond */
      #else /* FEATURE_WINCE */
        #error code not present
#endif /* FEATURE_WINCE */
    
      #ifndef INLINE
        /** @cond
        */
        #define INLINE inline
        /** @endcond */
      #endif
    #endif /* defined (__GNUC__) */

  /** @cond
  */
  #define far
  #define near
  #define _far
  #define _near
  #define _pascal
  #define _interrupt
  /** @endcond */

#else /* T_WINNT || TARGET_OS_SOLARIS || __GNUC__ */

  /* WINNT or SOLARIS based targets*/
  #if defined (__GNUC__) || defined (__GNUG__)

    /*For Q6, we need to define __packed since RPC autogenerated
     files are using __packed. Define it as a dummy to get past the
     compilation*/
     #ifdef FEATURE_QDSP6
     #ifndef __packed
     #define __packed
     #endif /*__packed*/
     #endif /*FEATURE_QDSP6*/

    /* The macros PACKED and __packed are deprecated.  The following lines will
     * create compiler warnings.  Change PACKED to use PACK() instead.
    */
    /** @cond
    */
    #define PACKED 
    /** @endcond */
    #define ALIGN(__value) __attribute__((aligned(__value)))
    #define POST_ALIGN(__value)
    #define PACKED_POST __attribute__((__packed__)) 

     // workarounds to lack of GCC support for pointer-to-unaligned-intrinsic
        typedef struct __attribute__((packed))
        { uint16 x; }
        unaligned_uint16;
        typedef struct __attribute__((packed))
        { uint32 x; }
        unaligned_uint32;
        typedef struct __attribute__((packed))
        { uint64 x; }
        unaligned_uint64;
        typedef struct __attribute__((packed))
        { int16 x; }
        unaligned_int16;
        typedef struct __attribute__((packed))
        { int32 x; }
        unaligned_int32;
        typedef struct __attribute__((packed))
        { int64 x; }
        unaligned_int64;
        #define PACKED_UINT16 unaligned_uint16
        #define PACKED_UINT16_ACCESS(PTR) ((unaligned_uint16 *) (PTR))->x
        #define PACKED_UINT32 unaligned_uint32
        #define PACKED_UINT32_ACCESS(PTR) ((unaligned_uint32 *) (PTR))->x
        #define PACKED_UINT64 unaligned_uint64
        #define PACKED_UINT64_ACCESS(PTR) ((unaligned_uint64 *) (PTR))->x
        #define PACKED_INT16 unaligned_int16
        #define PACKED_INT16_ACCESS(PTR) ((unaligned_int16 *) (PTR))->x
        #define PACKED_INT32 unaligned_int32
        #define PACKED_INT32_ACCESS(PTR) ((unaligned_int32 *) (PTR))->x
        #define PACKED_INT64 unaligned_int64
        #define PACKED_INT64_ACCESS(PTR) ((unaligned_int64 *) (PTR))->x
       
  #else
    #define PACKED
    #define __packed
    #define ALIGN(__value)
    #define POST_ALIGN(__value)
    #define PACKED_POST
    #define inline __inline
    /* Ignore gcc-style attributes on msvc builds */
    #define __attribute__(x)

    #define PACKED_UINT16 uint16
    #define PACKED_UINT16_ACCESS(PTR) (*((uint16*)PTR))
    #define PACKED_UINT32 uint32
    #define PACKED_UINT32_ACCESS(PTR) (*((uint32*)PTR))
    #define PACKED_UINT64 uint64
    #define PACKED_UINT64_ACCESS(PTR) (*((uint64*)PTR))
    #define PACKED_INT16 int16
    #define PACKED_INT16_ACCESS(PTR) (*((int16*)PTR))
    #define PACKED_INT32 int32
    #define PACKED_INT32_ACCESS(PTR) (*((int32*)PTR))
    #define PACKED_INT64 int64
    #define PACKED_INT64_ACCESS(PTR) (*((int64*)PTR))
  
  #endif

  #ifndef INLINE

    /* INLINE is defined to __inline because WINNT targets work fine with it
     * and defining it to nothing does not work because the inline function
     * definition is then multiply defined.  Solaris may need a different value.
     */
    #ifdef __GNUC__
      #define INLINE static inline
    #else
      #define INLINE __inline
    #endif /*  __GNUC__ */
  #endif /*  INLINE  */
  #define _pascal
  #define _far
  #define far
  #define near
  #define _near

  #define _interrupt
  #define _fmemcpy memcpy

#endif /* T_WINNT */

#ifndef CDECL
  #ifdef _MSC_VER
    #define CDECL __cdecl
  #else
    /** @cond
    */
    #define CDECL
    /** @endcond */
  #endif /* _MSC_VER */
#endif /* CDECL */
/** @cond
*/
#define cdecl
#ifndef _cdecl
#define _cdecl
#endif
/** @endcond */

/** @name Common Macros
@{ */

/*--------------- The __NON_DEMAND_PAGED_FUNCTION__ macro -----------------*/
/**
  Allows for a specific function in a file to be placed in a non-demand 
  paged memory section in the memory map, even if the file itself was set 
  to be demand paged.
 
  @param[in] function Function to be placed in a non-demand paged memory 
                      section.

  @return
  None.

  @dependencies
  This macro is enabled by FEATURE_NON_DEMAND_PAGED_FUNCTION. If the
  feature is off, the macro is defined as NULL.
*/
/*------------ BEGIN __NON_DEMAND_PAGED_FUNCTION__ Definition -------------*/
#if defined(FEATURE_NON_DEMAND_PAGED_FUNCTION) && defined(__ARMCC_VERSION)
  #error code not present
#else
  #define __NON_DEMAND_PAGED_FUNCTION__
  /** @cond
  */
  #define __NON_DEMAND_PAGED_FUNCTION_END__
  /** @endcond */
#endif // #if defined(FEATURE_NON_DEMAND_PAGED_FUNCTION) && defined(__ARMCC_VERSION)
/*------------ END __NON_DEMAND_PAGED_FUNCTION__ Definition ---------------*/


/*------------------------- The PACK() macro ------------------------------*/
/**
  Sets up the semantics for the PACK() macro based upon the target compiler.

  PACK() is necessary to ensure portability of C variable/struct/union
  packing across many platforms. For example:
  - ARM compilers require the following:
<pre>
  typedef __packed struct { ... } foo_t;
</pre>
@latexonly \newpage @endlatexonly
  - GCC requires the following to achieve the same effect:
<pre>  
  typedef struct __attribute((__packed__)) struct { ... } foo_t;
</pre>

  - Microsoft VC++ requires an alignment #pragma prologue and epilogue.

  To satisfy all three requirements, the following form is recommended:

  <pre>
    #ifdef _WIN32
    #pragma pack(push,1) // Save previous, and turn on 1 byte alignment
    #endif

    typedef PACK(struct)
    {
      ...
    } such_and_such_t;

    typedef PACK(struct)
    {
      ...
    } this_and_that_t;

    typedef PACK(struct)
    {
      PACK(struct)
      {
        ...
      } hdr;
      PACK(union)
      {
        such_and_such_t sas;
        this_and_that_t tat;
      } payload;
    } cmd_t;

    #ifdef _WIN32
    #pragma pack(pop) // Revert alignment to what it was previously
    #endif
  </pre>
	
  @param[in] x Structure to be packed.
	
  @return
  None.
	
  @dependencies
  The form explained in the description is recommended.
*/
/*--------------------- BEGIN PACK() Definition -------------------------*/

#if defined __GNUC__
  #define PACK(x)       x __attribute__((__packed__))
#elif defined __GNUG__
  #define PACK(x)       x __attribute__((__packed__))
#elif defined __arm
  #define PACK(x)       __packed x
#elif defined _WIN32
  #define PACK(x)       x /* Microsoft uses #pragma pack() prologue/epilogue */
#else
  #define PACK(x)       x __attribute__((__packed__))
#endif

/*------------------- END PACK() Definition ------------------------------*/


/*----------------- The HANDPACK() macro ---------------------------------*/
/**
  Allows for hand-packing of structures/data on
  compilers that can be trusted to obey strict manual packing rules,
  thus avoiding the need for the coercive-packing PACK() macro.

  Usage of HANDPACK() is exactly the same as PACK(), with the caveat that
  it will become vestigial on compilers for which hand-packing is known to
  be safe.

  @param[in] x Structure to be packed
  
  @dependencies
  The main rule of hand-packing is that types of _known_ size must be used. 
  @par
  @latexonly \vspace{-0.15in} @endlatexonly
  Following are the valid sized types:
  - int8
  - uint8
  - int16
  - uint16
  - int32
  - uint32 
  @par
  @latexonly \vspace{-0.15in} @endlatexonly
  The following types are not allowed in a hand-packed structure:
  - int64
  - uint64
  - enum 
  @par
  @latexonly \vspace{-0.15in} @endlatexonly
  For example, if users must have a member that takes an enum type, 
  they declare the packed structure member as one of the above valid types,
  and cast the incoming value to that type upon assignment.

  @sa
  For further details, see PACK().
*/
/*---------------- BEGIN HANDPACK() Definition ----------------------------*/
#if defined __GNUC__ || defined __arm
  #define HANDPACK(x)               x
#else /* For all other compilers not special-cased above, enforce packing */
  #define HANDPACK(x)               PACK(x)
#endif
/*--------------- END HANDPACK() Definition ------------------------------*/



/* ----------------------------------------------------------------------
** Lint does not understand __packed, so we define it away here.  In the
** past we did this:
**   This helps us catch non-packed pointers accessing packed structures,
**   for example, (although lint thinks it is catching non-volatile pointers
**   accessing volatile structures).
**   This does assume that volatile is not being used with __packed anywhere
**   because that would make Lint see volatile volatile (grrr).
** but found it to be more trouble than it was worth as it would emit bogus
** errors
** ---------------------------------------------------------------------- */
#ifdef _lint
  #ifdef __packed
    #undef __packed
  #endif /* __packed */
  #define __packed
#endif

/* ----------------------------------------------------------------------
**                          STANDARD MACROS
** ---------------------------------------------------------------------- */

/* Macro to explicitly designate that a function is externalized.
** Only to be used in the scope of the .c file (in the header file the
** reserved C word "extern" is to be used).
**
** To be more explicit about local vs. externalized functions, each function
** must have "static" or "EXTERN" in front of its source file definition.
*/
/** @cond
*/
#ifndef EXTERN
#define EXTERN
#endif
/** @endcond */


/*===========================================================================

MACRO MEM_B
MACRO MEM_W
===========================================================================*/
/**
  Dereferences an address as a byte, allowing access to an
  arbitrary memory byte.
  
  @param[in] x Address to be dereferenced.

  @return
  The byte at the specified address.

  @dependencies
  None.
*/
#define  MEM_B( x )  ( *( (byte *) (x) ) )

/**
  Dereferences an address as a word, allowing access to an
  arbitrary memory word.
  
  @param[in] x Address to be dereferenced.

  @return
  The word at the specified address.

  @dependencies
  None.
*/
#define  MEM_W( x )  ( *( (word *) (x) ) )



/*===========================================================================

MACRO MAX
MACRO MIN
===========================================================================*/
/**
  Evaluates the maximum of two specified arguments.

  @param[in] x Parameter to be compared to y.
  @param[in] y Parameter to be compared to x.

  @return
  The greater of the two values.

  @dependencies
  None.
*/
#ifndef MAX
   #define  MAX( x, y ) ( ((x) > (y)) ? (x) : (y) )
#endif

/**
  Evaluates the minimum of two specified arguments.

  @param[in] x Parameter to be compared to y.
  @param[in] y Parameter to be compared to x.

  @return
  The lesser of the two values.

  @dependencies
  None.
*/
#ifndef MIN
   #define  MIN( x, y ) ( ((x) < (y)) ? (x) : (y) )
#endif



/*===========================================================================

MACRO FPOS
===========================================================================*/
/**
  Computes the offset, in bytes, of a specified field
  in a specific structure or union type.

  @param[in] type  Type of the structure or union.
  @param[in] field Field in the structure or union from which to get 
                   the offset.

  @return
  Byte offset of the field in the specified structure or union type.

  @dependencies
  None.

  @sideeffects
  The lint error, Warning 545: Suspicious use of &, is suppressed within
  this macro so that lint will not give an error when the field parameter 
  is an array.
*/
#define FPOS( type, field ) \
    /*lint -e545 */ ( (dword) &(( type *) 0)-> field ) /*lint +e545 */



/*===========================================================================

MACRO FSIZ
===========================================================================*/
/**
  Computes the size, in bytes, of a specified field
  in a specific structure or union type.
 
  @param[in] type  Type of the structure or union.
  @param[in] field Field in the structure or union from which to get 
                   the size.

  @return
  Size, in bytes, of the field in the specified structure or union type.

  @dependencies
  None.
*/
#define FSIZ( type, field ) sizeof( ((type *) 0)->field )



/*===========================================================================

MACRO FLIPW
===========================================================================*/
/**
  Takes a 2-byte array, with the most significant byte first, followed
  by the least significant byte, and converts the quantity into a word
  with Intel byte order (least significant byte first).

  @param[in] ray Array of two bytes to be converted.

  @return
  Word in Intel byte order.

  @dependencies
  None.
*/
#define  FLIPW( ray ) ( (((word) (ray)[0]) * 256) + (ray)[1] )



/*===========================================================================

MACRO FLOPW
===========================================================================*/
/**
  Reverses the bytes of a word value and writes them into a 2-byte array.
  Effectively, this macro is the reverse of the FLIPW() macro.
 
  @param[in] ray Array that receives the two bytes from the word.
  @param[in] val Word that is broken into two bytes and put into the array.

  @return
  None.

  @dependencies
  None.
*/
#define  FLOPW( ray, val ) \
  (ray)[0] = ((val) / 256); \
  (ray)[1] = ((val) & 0xFF)



/*===========================================================================

MACRO B_PTR
MACRO W_PTR
===========================================================================*/
/**
  Casts the address of a specified variable as a pointer to a byte,
  allowing byte-wise access.
 
  @param[in] var Data for which to get a byte pointer.

  @return
  Byte pointer to the data.

  @dependencies
  None.
*/
#define  B_PTR( var )  ( (byte *) (void *) &(var) )
/**
  Casts the address of a specified variable as a pointer to a word,
  allowing word-wise access.
 
  @param[in] var Data for which to get a word pointer.

  @return
  Word pointer to the data.

  @dependencies
  None.
*/
#define  W_PTR( var )  ( (word *) (void *) &(var) )
/**
  Casts the address of a specified variable as a pointer to a dword,
  allowing dword-wise access.
 
  @param[in] var Data for which to get a dword pointer.

  @return
  Dword pointer to the data.

  @dependencies
  None.
*/
#define  D_PTR( var )  ( (dword *) (void *) &(var) )


/*===========================================================================

MACRO WORD_LO
MACRO WORD_HI
===========================================================================*/
/**
  Takes a word and extracts the least significant byte.

  @param[in] xxx Word from which to extract the least significant byte.

  @return
  Least significant byte.

  @dependencies
  None.
*/
#define  WORD_LO(xxx)  ((byte) ((word)(xxx) & 255))
/**
  Takes a word and extracts the most significant byte.

  @param[in] xxx Word from which to extract the most significant byte.

  @return
  Most significant byte.

  @dependencies
  None.
*/
#define  WORD_HI(xxx) \
  /*lint -e572 -e778 */  ((byte) ((word)(xxx) >> 8))  /*lint +e572 +e778 */
 /* Constant expression evaluates to 0 in operation '>>'*/


/*===========================================================================

MACRO RND8
===========================================================================*/
/**
  Rounds a number up to the nearest multiple of 8.

  @param[in] x Number to be rounded up.

  @return
  Number rounded up.

  @dependencies
  None.
*/
#define RND8( x )       ((((x) + 7) / 8 ) * 8 )


/*===========================================================================

MACRO INTLOCK / INTFREE
===========================================================================*/
#if defined T_WINNT
   #error code not present
#elif defined FEATURE_WINCE_OAL
  /* WM kernel manages interrupts itself using native windows system calls */
  #define INTLOCK()
  #define INTFREE()

#elif !defined _ARM_ASM_ && (!defined FEATURE_BLAST || defined ARCH_QDSP6)

  #ifdef __cplusplus
    extern "C" {
  #endif
  /** @cond 
  */
  extern dword rex_int_lock(void);
  extern dword rex_int_free(void);
  /** @endcond */
  #ifdef FEATURE_WINCE
    #error code not present
#endif

  #ifdef __cplusplus
    }
  #endif

  #if (!defined FEATURE_NO_REX) && (!defined INTLOCK)
    #if defined BUILD_BOOT_CHAIN || defined BUILD_TOOL_CHAIN || defined BUILD_BOOT_CHAIN_SPBL
       #define  INTLOCK()
       #define  INTFREE()
    #else
      /**
      Saves the current interrupt state on the stack, and then informs the 
      kernel not to process interrupts in this thread. One interrupt may still 
      occur, but it will not be processed until either INTFREE() is called or a 
      context switch is made to another thread that has interrupts enabled.

      @note1hang 
      INTLOCK/INTFREE may be nested, in which case inner INTLOCK/INTFREE
      pairs have no effect.

      @return
      None.

      @dependencies
      None.
	  */
      #define  INTLOCK( )  { dword sav = rex_int_lock();
      /**
      Restores the previous interrupt state from the stack. This macro is used
      in conjunction with INTLOCK().

      @note1hang
      INTLOCK/INTFREE may be nested, in which case inner INTLOCK/INTFREE
      pairs have no effect.

      @return
      None.

      @dependencies
      None.
	  */
      #define  INTFREE( )  if(!sav) (void)rex_int_free();}

    #endif
  #endif

#endif

/*===========================================================================

MACRO FIQLOCK / FIQFREE
===========================================================================*/
#ifdef ARCH_QDSP6
  /**
  Saves the current FIQ state on the stack, then disables FIQ interrupts.
  This macro is used in conjunction with FIQFREE().
  
  @return
  None.

  @dependencies
  Currently, this macro is available only in L4-based builds.
  */
  #define FIQLOCK()
  /**
  Restores the previous FIQ interrupt state from the stack. This macro is
  used in conjunction with FIQLOCK().

  @return
  None.

  @dependencies
  Currently, this macro is available only in L4-based builds.
  */
  #define FIQFREE()

#endif

/*===========================================================================

MACRO INTLOCK_SAV / INTFREE_SAV
===========================================================================*/


#ifdef _lint    /* get lint to 'know' the parameter is accessed */

  #define  INTLOCK_SAV(sav_var)   (sav_var = 1)
  #define  INTFREE_SAV(sav_var)   (sav_var = sav_var + 1)

#else

   #if defined T_WINNT
#error code not present
#elif !defined _ARM_ASM_
     #if defined BUILD_BOOT_CHAIN  || defined BUILD_TOOL_CHAIN || defined BUILD_BOOT_CHAIN_SPBL
       #define  INTLOCK_SAV(sav_var )
       #define  INTFREE_SAV(sav_var )
     #else
     #ifndef INTLOCK_SAV
	   /**
	   Saves the current interrupt state in the specified variable sav_var,
       then disables interrupts. This macro is used in conjunction with 
	   INTFREE_SAV().

	   @param[out] sav_var Current flags register, including the interrupt 
                           status.

	   @return
	   None.
	   
	   @dependencies
	   None.
	   */
       #define  INTLOCK_SAV( sav_var )  sav_var = rex_int_lock()
     #endif
     #ifndef INTFREE_SAV
	   /**
	   Restores the previous interrupt state from the specified variable
       sav_var. This macro is used in conjunction with INTLOCK_SAV().

	   @param[in] sav_var Saved flags register, including the interrupt 
                          status.

	   @return
	   None.
	   
	   @dependencies
	   None.
	   */
       #define  INTFREE_SAV( sav_var )  if(!sav_var) rex_int_free()
     #endif
     #endif
   #endif
 
#endif /* END if _lint */

/*===========================================================================

MACRO TASKLOCK/TASKFREE/TASKLOCK_SAV/TASKFREE_SAV
===========================================================================*/
#if !defined _ARM_ASM_
#ifdef __cplusplus
   extern "C" void rex_task_lock( void);
   extern "C" void rex_task_free( void);
#else
   extern void rex_task_lock( void);
   extern void rex_task_free( void);
#endif
#endif  /* !_ARM_ASM_ */


#ifdef FEATURE_QDSP6
/**
  Prevents context switching from the point at which the macro is invoked
  until a matching TASKFREE() is invoked. 
  
  While task-locked, the current task is guaranteed not to be replaced by a 
  different task, even if the other task has a higher priority. However, 
  TASKLOCK() does not block interrupts from occurring. An ISR will preempt 
  the task, but even if the ISR causes a higher priority task to become ready, 
  control will still resume at the interrupted task.
 
  @return
  None.
 
  @dependencies
  While a TASKLOCK() is in effect, rex-wait() does not work as
  usual because the calling task is not suspended. \n
  @latexonly \vspace{0.05in} @endlatexonly
  TASKLOCK() and TASKFREE() have no effect if they are called from within 
  an ISR.
*/
#define TASKLOCK( ) { rex_task_lock( );

/**
  Re-enables context switching.

  @return
  None.

  @dependencies
  See TASKLOCK().
*/
#define TASKFREE( ) rex_task_free( ); }

/**
  Prevents context switching from the point at which the macro is invoked
  until a matching TASKFREE_SAV() is invoked.

  TASKLOCK_SAV() is similar to TASKLOCK(), except that the TASKFREE_SAV() that 
  matches a TASKLOCK_SAV() does not have to be at the same static scope 
  within a function. This makes it easier to enable task scheduling and return 
  from the middle of a function.
 
  @return
  None.

  @dependencies
  None.
*/
#define TASKLOCK_SAV( ) rex_task_lock( )
/**
  Re-enables context switching.

  TASKFREE_SAV() is similar to TASKFREE(), except that the TASKFREE_SAV() that 
  matches a TASKLOCK_SAV() does not have to be at the same static scope 
  within a function. This makes it easier to enable task scheduling and return 
  from the middle of a function.
 
  @return
  None.

  @dependencies
  None.
*/
#define TASKFREE_SAV( ) rex_task_free( )

#endif /* T_WINNT */

/*===========================================================================

MACRO UPCASE
===========================================================================*/
/**
  Converts a character to uppercase; the character does not have to
  be printable or a letter.
 
  @param[in] c Character to be converted.

  @return
  Converted character.

  @dependencies
  The argument is referenced multiple times and must remain the same 
  value each time it is evaluated.

*/
#define  UPCASE( c ) ( ((c) >= 'a' && (c) <= 'z') ? ((c) - 0x20) : (c) )


/*===========================================================================

MACRO DECCHK
MACRO HEXCHK

DESCRIPTION
  These character attribute macros are similar to the standard 'C' macros
  (isdec and ishex), but do not rely on the character attributes table used
  by Microsoft 'C'.
===========================================================================*/
/**
  Checks if a character is a decimal digit.

  @param[in] c Character to be examined.

  @return
  TRUE -- Character is a decimal digit. \n
  FALSE -- Character is not a decimal digit.

  @dependencies
  None.
*/
#define  DECCHK( c ) ((c) >= '0' && (c) <= '9')

/**
  Checks if a character is a hexidecimal digit.

  @param[in] c Character to be examined.

  @return
  TRUE -- Character is a hexidecimal digit. \n
  FALSE -- Character is not a hexidecimal digit.

  @dependencies
  None.
*/
#define  HEXCHK( c ) ( ((c) >= '0' && (c) <= '9') ||\
                       ((c) >= 'A' && (c) <= 'F') ||\
                       ((c) >= 'a' && (c) <= 'f') )


/*===========================================================================

MACRO INC_SAT
===========================================================================*/
/**
  Increases a value, but saturates it at its maximum positive value and 
  does not let the value wrap back to 0 (unsigned) or negative (signed).
 
  @param[in] val Value to be increased with saturation.

  @return
  Increased value.

  @dependencies
  None.
*/
#define  INC_SAT( val )  (val = ((val)+1 > (val)) ? (val)+1 : (val))


/*===========================================================================

MACRO ARR_SIZE
===========================================================================*/
/**
  Returns the number of elements in an array.

  @param[in] a Array to be queried.

  @return
  Number of elements.

  @dependencies
  None.
*/
#ifndef ARR_SIZE
#define  ARR_SIZE( a )  ( sizeof( (a) ) / sizeof( (a[0]) ) )
#endif
/*===========================================================================
                     MACRO MOD_BY_POWER_OF_TWO

     Will calculate x % y, where x is a non-negative integer and
     y is a power of 2 from 2^0..2^32.

     Will work for 2^0, 2^1, 2^2, 2^3, 2^4, ... 2^32
      ie.            1,   2,   4,   8,  16, ... 4294967296
===========================================================================*/
/**
  Calculates x % y, where x is a non-negative integer, and y is a power of 2
  from 2^0 to 2^32 (inclusive).
 
  @param[in] x Value to be modified.
  @param[in] y Value by which to modify.

  @return
  x % y

  @dependencies
  See the description.
*/
#define MOD_BY_POWER_OF_TWO( val, mod_by ) \
           ( (dword)(val) & (dword)((mod_by)-1) )

/* -------------------------------------------------------------------------
**                       Debug Declarations
** ------------------------------------------------------------------------- */

/* Define 'SHOW_STAT' in order to view static's as globals
** (e.g. cl /DSHOW_STAT foo.c) If 'SHOW_STAT' is not defined,
** it gets defined as 'static'
*/
#ifdef LOCAL
   #undef LOCAL
#endif

/** @cond
*/
/* Permanently turn on SHOW_STAT since this has always been the case anyway
*/

#define LOCAL

/* Provide a new macro that will hopefully be used correctly to conditionally
** turn static on/off
*/
#ifndef T_WINNT
  #define STATIC static
#endif // !T_WINNT
/** @endcond */


/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/



/*===========================================================================

FUNCTION inp, outp, inpw, outpw, inpdw, outpdw

DESCRIPTION
  IN/OUT port macros for byte and word ports, typically inlined by compilers
  which support these routines
===========================================================================*/
#ifndef T_WINNT
  /* ARM based targets use memory mapped i/o, so the inp/outp calls are
  ** macroized to access memory directly
  */
  /**
  Reads a byte from the specified port address.
  
  @param[in] port Address of the port to be read (may be memory mapped).
  
  @return
  The read byte.
  
  @dependencies
  None.
  */
  #define inp(port)         (*((volatile byte *) (port)))
  /**
  Reads a word from the specified port address.
  
  @param[in] port Address of the port to be read (may be memory mapped).
  
  @return
  The read word.
  
  @dependencies
  None.
  */
  #define inpw(port)        (*((volatile word *) (port)))
#endif // !T_WINNT
  /**
  Reads a dword from the specified port address.
  
  @param[in] port Address of the port to be read (may be memory mapped).
  
  @return
  The read dword.
  
  @dependencies
  None.
  */
  #define inpdw(port)       (*((volatile dword *)(port)))

#ifndef T_WINNT
  /**
  Writes a byte to the specified port address.
  
  @param[in] port Address of the port in which to write (may be memory mapped).
  @param[in] val  Byte to write to the address.
  
  @return
  The written byte.

  @dependencies
  None.
  */
  #define outp(port, val)   (*((volatile byte *) (port)) = ((byte) (val)))
  /**
  Writes a word to the specified port address.
  
  @param[in] port Address of the port in which to write (may be memory mapped).
  @param[in] val  Word to write to the address.
  
  @return
  The written word.

  @dependencies
  None.
  */
  #define outpw(port, val)  (*((volatile word *) (port)) = ((word) (val)))
#endif // !T_WINNT
  /**
  Writes a dword to the specified port address.
  
  @param[in] port Address of the port in which to write (may be memory mapped).
  @param[in] val  Dword to write to the address.
  
  @return
  The written dword.

  @dependencies
  None.
  */
  #define outpdw(port, val) (*((volatile dword *) (port)) = ((dword) (val)))




/*===========================================================================

FUNCTION enable, disable

DESCRIPTION
  Interrupt enable and disable routines.  Enable should cause the CPU to
  allow interrupts and disable should cause the CPU to disallow
  interrupts.
===========================================================================*/
/** @cond 
*/
#ifndef T_WINNT

  #if !defined _ARM_ASM_
    /* ARM has no such definition, so we provide one here to enable/disable
    ** interrupts
    */
    #define _disable() (void)rex_int_lock()
    #define _enable()  (void)rex_int_free()

  #endif
#endif
/** @endcond */



#ifdef __cplusplus
}
#endif

/** @} */ /* end_name_group Common Macros */
/** @} */ /* end_addtogroup util_services */

#endif  /* COMDEF_H */
