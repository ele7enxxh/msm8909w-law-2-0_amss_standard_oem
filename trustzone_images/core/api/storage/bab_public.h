/***********************************************************************
 * FEATURE_BABYLON
 * bab_public.h
 *
 * BABYLON protocol public API.  Only this header file needs to be
 * included by code wishing to use Babylon logging calls.
 *
 * These are all the public functions provided by FEATURE_BABYLON.
 * Callers must use no functions except those listed in this file.
 *
 * Copyright (C) 2006-2010 Qualcomm Technologies Incorporated. All Rights Reserved.
 * Qualcomm Confidential and Proprietary.
 *
 ***********************************************************************/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/storage/bab_public.h#1 $

YYYY-MM-DD    who     what, where, why
----------    ---     ----------------------------------------------------------
2010-04-28    rh      Compilation error fix, remove use of BAB_TYPE_TASK_SCHEDULING_EVENT
2010-02-10    sp      Fixed couple of compilation errors
2009-05-19    sh      Adapted for gcc __inline__
2009-04-27    jm      Use __inline instead of inline
2009-01-16    sh      Clean-up for release; improve inlining
2008-11-07    rh      Adding seperate function for Marker Start/End
2008-11-06    sh      Comment cleanup (Stream ID)
2008-10-27    rh      Adding support for Windows Mobile Targets.
2006-05-15    sh      Added bab_port_test()
2006-01-06    sh      Created module from Alan C Wong's project.

===========================================================================*/
#ifndef __BAB_PUBLIC_H__
#define __BAB_PUBLIC_H__

/* We need this to get uint32 and friends */
#include "comdef.h"

#ifndef FEATURE_WINCE
/* All this to get our feature FEATURE_BABYLON flag, unfortunately.
 * Even more unfortunately, WM can't use customer.h file */
#include "customer.h"
#endif

/* Different compilers have different keywords for "inline":
 * GNU (gcc) uses __inline__, while Win32/WinCE and ARM use __inline */
#ifdef __GNUC__
  #define _INLINE_ __inline__
#else
  #define _INLINE_ __inline
#endif

#include "bab_defines.h"        /* Types and protocol defines */
#include "bab_streams.h"        /* Global Stream ID assignments */
#include "bab_internal.h"       /* Private helper functions */

/* There can be no sharing of the hardware port, and TIMETEST
 * uses the same Software Debug interface, so forbid it. */
#if defined(FEATURE_TIMETEST) || defined (TIMETEST)
  #error "FEATURE_BABYLON can not coexist with FEATURE_TIMETEST"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Send out a "SINGLE EVENT" packet.  These messages stand alone, and
 * communicate a single milestone in execution.
 *
 * @param stream
 * Stream ID
 *
 * @param id
 * To differentiate between events
 *
 * @param extended
 * Message ID; Developer-defined meaning
 */
#ifdef FEATURE_WINCE
void
bab_single_event (uint8 stream, uint8 id, uint8 extended);
#else
static _INLINE_ void
bab_single_event (uint8 stream, uint8 id, uint8 extended)
{
  _bab_write_packet (BAB_TYPE_SINGLE_EVENT << 8 | stream,
                     extended << 8 | id,
                     0, BAB_PAYLOAD_NONE);
}
#endif

/**
 * Send out a MARKER START packet, indicating the beginning of a
 * region of interest.  All Marker START packets must be matched by a
 * later Marker END packet for correct processing.
 *
 * @param stream
 * Stream ID
 *
 * @param id
 * Marker ID, to differentiate between markers.
 * 
 * @param extended
 * Extended ID, used for context info
 */
#ifdef FEATURE_WINCE
void
bab_marker_start (uint8 stream, uint8 id, uint8 extended);
#else
static _INLINE_ void
bab_marker_start (uint8 stream, uint8 id, uint8 extended)
{
  _bab_write_packet (BAB_TYPE_MARKER_START << 8 | stream,
                     extended << 8 | id,
                     0, BAB_PAYLOAD_NONE);
}
#endif

/**
 * Send out a Marker END packet.  These need to be paired
 * one-for-one with each Marker START packet for correct decoding.
 * 
 * @param stream
 * Stream ID
 *
 * @param id
 * Marker ID, to differentiate between markers.
 * 
 * @param extended
 * Extended ID, used for context info
 */
#ifdef FEATURE_WINCE
void
bab_marker_end (uint8 stream, uint8 id, uint8 extended);
#else
static _INLINE_ void
bab_marker_end (uint8 stream, uint8 id, uint8 extended)
{
  _bab_write_packet (BAB_TYPE_MARKER_END << 8 | stream,
                     extended << 8 | id,
                     0, BAB_PAYLOAD_NONE);
}
#endif

/* Marker by flag.. DEPRECATED.
 * The improved functions are faster and allow an extended ID as well. */
#ifndef FEATURE_WINCE
static _INLINE_ void
bab_marker (uint8 stream, uint8 id, uint8 state)
{
  _bab_write_packet (((state == BAB_MARKER_BEGIN) ? 
                      BAB_TYPE_MARKER_START : BAB_TYPE_MARKER_END) << 8 | stream,
                     id,        /* marker id (NO extended id)*/
                     0, BAB_PAYLOAD_NONE);
}
#endif

/**
 * Sends out a VALUE packet.  Babylon can efficiently send 16- or
 * 32-bit integer values.  The associated flags tell the receiving
 * side how they should be decoded and displayed.
 * 
 * @param stream
 * Stream ID
 * 
 * @param value_id
 * To differentiate between values as needed
 * 
 * @param value
 * integer value to send
 * 
 * @param is_decimal_display
 * TRUE to display in decimal, FALSE for hex
 * 
 * @param is_signed
 * TRUE if value is signed, FALSE if unsigned
 * 
 * @param is_32bit
 * TRUE if value is 32-bits, FALSE for 16-bit
 */
#ifdef FEATURE_WINCE
void
bab_value (uint8 stream,
           uint8 value_id,
           uint32 value,
           boolean is_decimal_display,
           boolean is_signed,
           boolean is_32bit);
#else
static _INLINE_ void
bab_value (uint8 stream,
           uint8 value_id,
           uint32 value,
           boolean is_decimal_display,
           boolean is_signed,
           boolean is_32bit)
{
  _bab_write_packet (BAB_TYPE_VALUE << 8 | stream,
                     /* The extended ID is actually the Format flag bits */
                     ((  is_decimal_display ? BAB_VALUE_FORMAT_DECIMAL : 0)
                      | (is_signed          ? BAB_VALUE_FORMAT_SIGNED : 0)
                      | (is_32bit           ? BAB_VALUE_FORMAT_32BIT : 0)) << 8
                     | value_id, /* value id */
                     value,     /* payload */
                     is_32bit ? BAB_PAYLOAD_32BIT : BAB_PAYLOAD_16BIT);
}
#endif

/* The same applies to the special case of Return values */
#ifdef FEATURE_WINCE
void
bab_return_value (uint8 stream,
                  uint8 value_id,
                  uint32 value,
                  boolean is_decimal_display,
                  boolean is_signed,
                  boolean is_32bit);
#else
static _INLINE_ void
bab_return_value (uint8 stream,
                  uint8 value_id,
                  uint32 value,
                  boolean is_decimal_display,
                  boolean is_signed,
                  boolean is_32bit)
{
  _bab_write_packet (BAB_TYPE_RETURN_VALUE << 8 | stream,
                     /* The extended ID is actually the Format flag bits */
                     ((  is_decimal_display ? BAB_VALUE_FORMAT_DECIMAL : 0)
                      | (is_signed          ? BAB_VALUE_FORMAT_SIGNED : 0)
                      | (is_32bit           ? BAB_VALUE_FORMAT_32BIT : 0)) << 8
                     | value_id, /* value id in the same word */
                     value,      /* payload */
                     is_32bit ? BAB_PAYLOAD_32BIT : BAB_PAYLOAD_16BIT);
}
#endif

/**
 * This function prepares the Babylon code for use.  In Windows
 * Mobile, it maps the physical address of the FPGA_LED port so the
 * Babylon functions can access it.
 */
#ifdef FEATURE_WINCE
void
bab_port_init (void);
#else
static _INLINE_ void
bab_port_init (void) 
{
  /* This is to initialize port access, which is done only in private functions */
  _bab_port_init ();
}
#endif

/**
 * Send out an ASCII string for informational display.
 *
 * This function is higher-overhead than any other, and will generate
 * one Babylon message for every 5 characters of the string.
 *
 * It should be used SPARINGLY, as long strings or frequently use can
 * add significant volume to the Babylon data stream, overflow FIFOs,
 * or slow runtime performance.
 *
 * The time taken to call this function is variable, depending on the
 * length of the string.
 *
 * @param stream
 * Stream ID
 *
 * @param id
 * String ID
 *
 * @param string
 * An NULL-terminated ASCII string of arbitrary length.
 */
#ifndef FEATURE_WINCE
#ifdef FEATURE_BABYLON
extern void bab_print_str (uint8 stream, uint8 id, const char *string);
#else
static _INLINE_ void
bab_print_str (uint8 stream, uint8 id, const char *string) 
{
  (void) stream;
  (void) id;
  (void) string;
}
#endif
#endif

/**
 * This function sends out INVALID packets for the purpose
 * of testing physical connectivity to the Bab port.
 * These are not valid BABYLON words, just recognizeable
 * patterns to ensure the bits are connected.
 * 
 * This should only be used to study the Babylon link itself, and
 * never called during normal use.
 */
#ifdef FEATURE_WINCE
void
bab_port_test (void);
#else
static _INLINE_ void
bab_port_test (void)
{
  /* This needs direct port access, which is done only in private functions */
  _bab_port_test ();
}
#endif
/**
 * Send out an SCHEDULING/TASK EVENT packet - This API has been removed, please use
 * bab_signle_event API instead.  Calling this API will not send any message.
 *
 */
#ifndef FEATURE_WINCE
static _INLINE_ void
bab_scheduling_event (uint8 stream,
                      uint8 event_id,
                      uint8 extended,
                      uint32 payload,
                      uint16 payload_size)
{
}
#endif

/**
 * Send out a raw packet, constructed from elements.  This is
 * generally only useful for constructing new or experimental packet
 * types.  The caller controls all contents.
 */
#ifndef FEATURE_WINCE
static _INLINE_ void
bab_raw_packet (uint8 type,
                uint8 stream,
                uint8 id,
                uint8 extended,
                uint32 payload,
                uint16 payload_size)
{
  _bab_write_packet (type << 8 | stream,
                     extended << 8 | id,
                     payload, payload_size);
}
#endif

#ifdef __cplusplus
}
#endif

/*
 * These macros are obsolete.  Do not use them.
 *
 * You may call babylon functions directly at all times, and the calls
 * will reduce to nothing (zero overhead, zero runtime) when the
 * feature is not enabled.
 *
 * These macros are preserved here for compatibility with old code
 * that used them.
 */
#define BAB_VALUE                  bab_value
#define BAB_RETURN_VALUE           bab_return_value
#define BAB_MARKER                 bab_marker
#define BAB_IRQ_EVENT(a,b,c,d,e)   (void)0 /* Obsolete */
#define BAB_SINGLE_EVENT           bab_single_event
#define BAB_SCHEDULING_EVENT       bab_scheduling_event
#define BAB_PRINT_STR              bab_print_str
#define BAB_SEND_GENERAL_PACKET    bab_send_general_packet

#endif /* __BAB_PUBLIC_H__ */

