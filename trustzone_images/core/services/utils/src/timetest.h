#ifndef TIMETEST_H
#define TIMETEST_H
/*===========================================================================

           T I M E T E S T   H E A D E R    F I L E

DESCRIPTION
  This header file contains all the definitions necessary for supporting
  timing tests via the LED output buffer.

Copyright (c) 1993, 1995, 1996 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 1997, 1998, 1999 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/



/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/services/utils/src/timetest.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/11/07   gfr     Include timetest_winCE.h for Windows.
04/09/07   ptm     Removed bio_led.h
02/27/07   gfr     Include the right MSM specific files.
10/26/06    ws     Updated TIMETEST_PORT if FEATURE_GSM_PLT is defined
08/21/06    dp     Timetest changes
04/25/06    dp     Integrated Ian's timetest changes
05/31/05    cr     Add support for dual LED port TIMETEST. No spinlock required
                   in dual port case. No shared memory led port shadow variable 
                   required in dual port case.
01/31/05    cr     Add support for 2 CPU. Created timetest_m7500.h and 
                   timetest_a7500.h to hold the values for ISR and TASK IDs
                   depending on which processor is compiling.
                   Added a bit in LED port to indicate which processor wrote
                   the LED port.
12/10/03    ss     Added timetest IDs for MPEG4 tasks
08/08/03   kvd     Modified TIMETEST_PORT and TIMETEST_DSRLP_BYTES macro to
                   support CPU and RLP throughput profiling on Jaguar
04/04/02    vr     Added profiling support for ONCRPC test app
08/14/01    om     Changed TIMETEST_DSRLP_ macro to distinguish 1x and HDR.
08/02/01    om     Added PPS value for 510x.
07/25/01    om     Added support for RLP Rx/Tx statistics.
07/16/01    om     Added task specific profiling support.
03/07/01   rmd     Updated file to support MSM510x surf.
02/13/99    ms     Removed pre-MSM2 definitions. Added defs for MSM3.
11/25/97   jjn     Instead of defining TIMETEST_PORT as 0x03E just for
                   Gemini, defined TIMETEST_PORT as 0x03E for all targets
                   that use MSM 2 family CDMA chip
04/04/95   jah     Added Gemini support
12/09/93   jah     Added comments
05/27/93   jah     Added Beta II support
02/16/93   jah     Initial revision.

===========================================================================*/

#include "target.h"
#include "bio.h"

/*===========================================================================

                      TIMETEST CONFIGURATION

===========================================================================*/


#ifdef TIMETEST
  /* Use different ports for the apps and modem processors */
  #define FEATURE_MULTIPORT_TIMETEST

  /* Multiple task / ISR profiling */
  #define TIMETEST_PROF

  /* Function level profiling */
  #undef TIMETEST_FUNCS

  /* Function level profiling only (no tasks / ISRs) */
  #undef TIMETEST_NO_TASKS

  /* Only count total ISR time, not individual ISR routines */
  #undef TIMETEST_NO_ISRS

  #define TIMETEST_DSRLP_STATS

  #undef TIMETEST_EXPLICIT_TRIGGER

  #define TIMETEST_ISR_LEVEL

  #ifndef FEATURE_L4
    /* backward compatibility */
    #define L4_Set_Profiling_ID(id)
  #endif
#endif



/*===========================================================================

                           INCLUDES

===========================================================================*/

#if (defined TIMETEST_FUNCS) && (! defined REXARM_S)
  #include "msg.h"
  #include "err.h"
  #include "assert.h"
#endif


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/* Define the address of the buffer which drives the outputs
** hooked to the profiling mux.
*/

#ifdef CPLD_HAS_LED_DBG

  /* TIMETEST Port address
  */
  #define TIMETEST_PORT  CPLD_LED_DEBUG_WH

#elif defined(T_MSM2) && !defined (T_MSM3)
  #ifdef T_MSM5200
    #define TIMETEST_PORT           0x03000000
  #else
    /* Gemini Portable*/
    #define TIMETEST_PORT           0x03E
  #endif

#else

  /* MSM3000 and up targets */

#if defined(TIMETEST) && defined(FEATURE_MULTIPORT_TIMETEST)
  #define TIMETEST_PORT             HWIO_ADDR(HAPPY_LED)
#else
/* since msm7500.builds/targ*.h assume multiport, timetest has
 * to explicity define away multiport if timetest is doing
 * single LED PORT TIMETEST data collection.
*/
  #ifdef T_RUMI
    #define TIMETEST_PORT             0x9c014000
  #elif defined(FEATURE_GSM_PLT)
    #define TIMETEST_PORT             0xFE510000
  #else
    #define TIMETEST_PORT             0x8C010000
  #endif
#endif
  #define TIMETEST_DSRLP_STATS
#endif /*CPLD_HAS_LED_DEBUG IS DEFINED*/

/* 16-bit value used as a unique parse token for the postprocessor */
#define TT_PARSE_TOKEN            0xffff
#define TT_PARSE_TOKEN_ISR_CONT   0x8000
#define TT_PARSE_TOKEN_ISR_LEVEL  0x0f00
#define TT_PARSE_TOKEN_IS856      0x4000
#define TT_PARSE_TOKEN_IS2K       0x2000
#ifdef TIMETEST_ISR_LEVEL
  #define TT_PARSE_TOKEN_APPS       0x1000
#else
  #define TT_PARSE_TOKEN_APPS       0x0100
#endif

/* Values used to distinguish between 1x and HDR data rates */
#define TIMETEST_DSRLP_IS856   TT_PARSE_TOKEN_IS856
#define TIMETEST_DSRLP_IS2K    TT_PARSE_TOKEN_IS2K

#ifndef IMAGE_APPS_PROC
  #include "timetest_msm_modem.h"
#else
  #include "timetest_msm_apps.h"
#endif /* !IMAGE_APPS_PROC */

#ifdef FEATURE_WINCE
  #include "timetest_winCE.h"
#endif

#define TIMETEST_INT_TASK_ID                 TIMETEST_MAX_TASK_ID
#define TIMETEST_ISR_NESTED_ID               0xff

/*** New timetest for function and ISR profiling ***/
#ifdef TIMETEST_PROF

#ifndef FEATURE_MULTIPORT_TIMETEST
  #define TT_LOCK() BIO_LED_LOCK()
  #define TT_FREE() BIO_LED_FREE()
#else
  #define TT_LOCK() INTLOCK()
  #define TT_FREE() INTFREE()
#endif

  /* No foward declarations in assembly code ! */
  #ifndef REXARM_S
    extern unsigned char timetest_isr_markers[32];
    extern unsigned char timetest_isr_level;
    extern unsigned long timetest_parse_token;
    extern void timetest_prof_init( void );
  #endif

  /* Macro to write an ISR's ID to the LED port */
  #ifndef TIMETEST_NO_ISRS
    #if defined(IMAGE_APPS_PROC) && !defined(FEATURE_MULTIPORT_TIMETEST)
      #ifdef TIMETEST_ISR_LEVEL
        #ifdef TIMETEST_EXPLICIT_TRIGGER
          #define TIMETEST_ISR_ID( id )                                        \
            TT_LOCK();                                                         \
            timetest_isr_markers[timetest_isr_level] = id;                     \
            outpw (TIMETEST_PORT, (uint16)(0x0000) );                          \
            BIO_LED_BUF =                                                      \
                   (uint16)(((timetest_isr_level & 0x0f) << 8)                 \
                            | id | TT_PARSE_TOKEN_APPS);                       \
            outpw( TIMETEST_PORT, BIO_LED_BUF );                               \
            TT_FREE()
        #else
            #define TIMETEST_ISR_ID( id )                                      \
            TT_LOCK();                                                         \
            timetest_isr_markers[timetest_isr_level] = id;                     \
            BIO_LED_BUF =                                                      \
                   (uint16)(((timetest_isr_level & 0x0f) << 8)                 \
                            | id | TT_PARSE_TOKEN_APPS);                       \
            outpw( TIMETEST_PORT, BIO_LED_BUF );                               \
            TT_FREE()
        #endif
      #else
        #ifdef TIMETEST_EXPLICIT_TRIGGER
          #define TIMETEST_ISR_ID( id )                                        \
            TT_LOCK();                                                         \
            timetest_isr_markers[timetest_isr_level] = id;                     \
            outpw (TIMETEST_PORT, (uint16)(0x0000) );                          \
            BIO_LED_BUF = (uint16)(id | TT_PARSE_TOKEN_APPS);                  \
            outpw( TIMETEST_PORT, BIO_LED_BUF );                               \
            TT_FREE()
        #else
            #define TIMETEST_ISR_ID( id )                                      \
            TT_LOCK();                                                         \
            timetest_isr_markers[timetest_isr_level] = id;                     \
            BIO_LED_BUF = (uint16)(id | TT_PARSE_TOKEN_APPS);                  \
            outpw( TIMETEST_PORT, BIO_LED_BUF );                               \
            TT_FREE()
        #endif
      #endif /* TIMETEST_ISR_LEVEL */
    #else
      #ifdef TIMETEST_ISR_LEVEL
        #ifdef TIMETEST_EXPLICIT_TRIGGER
          #define TIMETEST_ISR_ID( id )                                        \
            TT_LOCK();                                                         \
            timetest_isr_markers[timetest_isr_level] = id;                     \
            outpw (TIMETEST_PORT, (uint16)(0x0000) );                          \
            BIO_LED_BUF = (uint16)(((timetest_isr_level & 0x0f) << 8) | id);   \
            outpw( TIMETEST_PORT, BIO_LED_BUF );                               \
            TT_FREE()
        #else
            #define TIMETEST_ISR_ID( id )                                      \
            TT_LOCK();                                                         \
            timetest_isr_markers[timetest_isr_level] = id;                     \
            BIO_LED_BUF = (uint16)(((timetest_isr_level & 0x0f) << 8) | id);   \
            outpw( TIMETEST_PORT, BIO_LED_BUF );                               \
            TT_FREE()
        #endif
      #else
        #ifdef TIMETEST_EXPLICIT_TRIGGER
          #define TIMETEST_ISR_ID( id )                                        \
            TT_LOCK();                                                         \
            timetest_isr_markers[timetest_isr_level] = id;                     \
            outpw (TIMETEST_PORT, (uint16)(0x0000) );                          \
            BIO_LED_BUF = (uint16) (id);                                       \
            outpw( TIMETEST_PORT, BIO_LED_BUF );                               \
            TT_FREE()
        #else
            #define TIMETEST_ISR_ID( id )                                      \
            TT_LOCK();                                                         \
            timetest_isr_markers[timetest_isr_level] = id;                     \
            BIO_LED_BUF = (uint16) (id);                                       \
            outpw( TIMETEST_PORT, BIO_LED_BUF );                               \
            TT_FREE()
        #endif
      #endif /* TIMETEST_ISR_LEVEL */
    #endif /* IMAGE_APPS_PROC */
  #else
    #define TIMETEST_ISR_ID( id )
  #endif

  /* Value on the buffer output when in the Interrupt Handler */
  #define TIMETEST_INT   TIMETEST_MAX_TASK_ID

  /* Macro to write RLP rx and tx statistics out */
  /* This macro has been modified specific for 6500 */
  #ifdef TIMETEST_EXPLICIT_TRIGGER
    #define TIMETEST_DSRLP_BYTES( rlp_type, rxb, txb )                      \
      TT_LOCK();                                                            \
      outpw( TIMETEST_PORT, (uint16)(0x0000) );                             \
      outpw( TIMETEST_PORT, (uint16)(rlp_type) );                           \
      outpw( TIMETEST_PORT, (uint16)(0x0000) );                             \
      outpw( TIMETEST_PORT, ((((uint16)(rxb)) << 8) | txb ));               \
      TT_FREE();
  #else
    #define TIMETEST_DSRLP_BYTES( rlp_type, rxb, txb )                      \
      TT_LOCK();                                                            \
      outpw( TIMETEST_PORT, (uint16)(rlp_type) );                           \
      outpw( TIMETEST_PORT, ((((uint16)(rxb)) << 8) | txb ));               \
      TT_FREE();
  #endif

  /* Macros to write the function address upon beginning and   */
  /* end of a function body.                                   */
  #ifdef TIMETEST_FUNCS

  /* This is dependent on the assembly code in rexarm.s  */
  /* Don't change it !!                                  */
  #define TIMETEST_MAX_FUNC_LEVEL     31

  #ifndef TIMETEST_NO_TASKS
    #define TT_OUT_FADDR( tid, f_addr )   \
      outpw( TIMETEST_PORT, (uint16)(0x0000) );                               \
      outpw( TIMETEST_PORT, (uint16)(TT_PARSE_TOKEN) );                       \
      outpw( TIMETEST_PORT, (uint16)(0x0000) );                               \
      outpw( TIMETEST_PORT, (uint16)(tid) );                                  \
      outpw( TIMETEST_PORT, (uint16)(0x0000) );                               \
      outpw( TIMETEST_PORT, (uint16)((((uint32)f_addr) & 0xffff0000) >> 16) );\
      outpw( TIMETEST_PORT, (uint16)(0x0000) ); \
      outpw( TIMETEST_PORT, (uint16)(((uint32)f_addr) & 0x0000ffff) );        
  #else /* TIMETEST_NO_TASKS */
    #define TT_OUT_FADDR( tid, f_addr )   \
      outpw( TIMETEST_PORT, (uint16)(0x0000) );                               \
      outpw( TIMETEST_PORT, (uint16)(TT_PARSE_TOKEN) );                       \
      outpw( TIMETEST_PORT, (uint16)(0x0000) );                               \
      outpw( TIMETEST_PORT, (uint16)((((uint32)f_addr) & 0xffff0000) >> 16) );\
      outpw( TIMETEST_PORT, (uint16)(0x0000) ); \
      outpw( TIMETEST_PORT, (uint16)(((uint32)f_addr) & 0x0000ffff) );        
  #endif /* TIMETEST_NO_TASKS */

  #define TT_BEGIN_FUNC( f_addr )  \
    { TT_LOCK();                                                              \
    if ( ! rex_self()->leds ) ERR("LEDs not set for task %x",rex_self(),0,0); \
    else {                                                                    \
    ASSERT( timetest_markers[rex_self()->leds][0] < 31 );                     \
    timetest_markers[rex_self()->leds][0]++;                                  \
    timetest_markers[rex_self()->leds][timetest_markers[rex_self()->leds][0]] \
      = (uint32) f_addr;                                                      \
    TT_OUT_FADDR( rex_self()->leds, f_addr );                                 \
    } TT_FREE(); }

  #define TT_END_FUNC( f_addr )    \
    { TT_LOCK();                                                              \
    if ( rex_self()->leds && timetest_markers[rex_self()->leds][0] ){         \
    timetest_markers[rex_self()->leds][timetest_markers[rex_self()->leds][0]] \
      = 0x00;                                                                 \
    timetest_markers[rex_self()->leds][0]--;                                  \
    TT_OUT_FADDR( rex_self()->leds,                                           \
                  timetest_markers[rex_self()->leds]                          \
                                  [timetest_markers[rex_self()->leds][0]] );  \
    } TT_FREE(); }

  #ifndef REXARM_S
    /* Array to store markers during context switches */
    extern unsigned long timetest_markers[TIMETEST_MAX_TASK_ID]
                                         [TIMETEST_MAX_FUNC_LEVEL+1];

    extern void timetest_prof_init( void );
  #endif

  #endif /* (defined TIMETEST_FUNCS) */

#else  /* (defined TIMETEST_PROF) */

  /*** Legacy support for the old timetest ***/
  #define TIMETEST_ISR_ID( id )
  #define TIMETEST_INT    0x1f
  #if defined(T_MB_SURF_510X)
    #define TIMETEST_1PPS   0x20
  #else
    #define TIMETEST_1PPS   0x1E
  #endif

#endif  /* (defined TIMETEST_PROF) */

#endif  /* TIMETEST_H */



