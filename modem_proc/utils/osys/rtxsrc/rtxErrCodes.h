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
 * @file rtxErrCodes.h
 * List of numeric status codes that can be returned by common run-time
 * functions and generated code.
 */
#ifndef _RTXERRCODES_H_
#define _RTXERRCODES_H_

/**
 * @defgroup rtxErrCodes Run-time error status codes.
 * This is a list of status codes that can be returned by the common
 * run-time functions and generated code.  In many cases, additional
 * information and parameters for the different errors are stored in
 * the context structure at the time the error in raised.  This
 * additional information can be output using the \c rtxErrPrint or
 * \c rtxErrLogUsingCB run-time functions.
 * @{
 */

/**
 * Normal completion status.
 */
#define RT_OK            0

/**
 * Message fragment return status.  This is returned when a part of a
 * message is successfully decoded.  The application should continue
 * to invoke the decode function until a zero status is returned.
 */
#define RT_OK_FRAG       2

/**
 * Encode buffer overflow.  This status code is returned when encoding
 * into a static buffer and there is no space left for the item currently
 * being encoded.
 */
#define RTERR_BUFOVFLW  -1

/**
 * Unexpected end-of-buffer.  This status code is returned when decoding
 * and the decoder expects more data to be available but instead runs
 * into the end of the decode buffer.
 */
#define RTERR_ENDOFBUF  -2

/**
 * Expected identifier not found.  This status is returned when the
 * decoder is expecting a certain element to be present at the current
 * position and instead something different is encountered.  An example
 * is decoding a sequence container type in which the declared elements
 * are expected to be in the given order.  If an element is encountered
 * that is not the one expected, this error is raised.
 */
#define RTERR_IDNOTFOU  -3

/**
 * Invalid enumerated identifier.  This status is returned when an
 * enumerated value is being encoded or decoded and the given value
 * is not in the set of values defined in the enumeration facet.
 */
#define RTERR_INVENUM   -4

/**
 * Duplicate element in set.  This status code is returned when decoding
 * an ASN.1 SET or XSD xsd:all construct.  It is raised if a given element
 * defined in the content model group occurs multiple times in the instance
 * being decoded.
 */
#define RTERR_SETDUPL   -5

/**
 * Missing required element in set.  This status code is returned when
 * decoding an ASN.1 SET or XSD xsd:all construct and all required elements
 * in the content model group are not found to be present in the instance
 * being decoded.
 */
#define RTERR_SETMISRQ  -6

/**
 * Element not in set.  This status code is returned when encoding or
 * decoding an ASN.1 SET or XSD xsd:all construct.  When encoding, it
 * occurs when a value in the generated _order member variable is outside
 * the range of indexes of items in the content model group.  It occurs on
 * the decode side when an element is received that is not defined in the
 * content model group.
 */
#define RTERR_NOTINSET  -7

/**
 * Sequence overflow.  This status code is returned when decoding a
 * repeating element (ASN.1 SEQUENCE OF or XSD element with
 * min/maxOccurs > 1) and more instances of the element are received
 * than were defined in the constraint.
 */
#define RTERR_SEQOVFLW  -8

/**
 * Invalid option in choice.  This status code is returned when encoding or
 * decoding an ASN.1 CHOICE or XSD xsd:choice construct.  When encoding,
 * it occurs when a value in the generated 't' member variable is outside
 * the range of indexes of items in the content model group.  It occurs
 * on the decode side when an element is received that is not defined in
 * the content model group.
 */
#define RTERR_INVOPT    -9

/**
 * No dynamic memory available.  This status code is returned when a
 * dynamic memory allocation request is made and an insufficient amount
 * of memory is available to satisfy the request.
 */
#define RTERR_NOMEM     -10

/**
 * Invalid hexadecimal string.  This status code is returned when
 * decoding a hexadecimal string value and a character
 * is encountered in the string that is not in the valid hexadecimal
 * character set ([0-9A-Fa-f] or whitespace).
 */
#define RTERR_INVHEXS   -11

/**
 * Invalid real number value.  This status code is returned when
 * decoding a numeric floating-point value and an invalid character is
 * received (i.e. not numeric, decimal point, plus or minus sign, or
 * exponent character).
 */
#define RTERR_INVREAL   -12

/**
 * String overflow.  This status code is returned when a fixed-sized
 * field is being decoded as specified by a size constraint and
 * the item contains more characters or bytes then this amount.  It
 * can occur when a run-time function is called with a fixed-sixed
 * static buffer and whatever operation is being done causes the
 * bounds of this buffer to be exceeded.
 */
#define RTERR_STROVFLW  -13

/**
 * Bad value. This status code is returned anywhere where an API is
 * expecting a value to be within a certain range and it not within
 * this range.  An example is the encoding or decoding date values
 * when the month or day value is not within the legal range (1-12 for
 * month and 1 to whatever the max days is for a given month).
 */
#define RTERR_BADVALUE  -14

/**
 * Nesting level too deep.  This status code is returned when a preconfigured
 * maximum nesting level for elements within a content model group is
 * exceeded.
 */
#define RTERR_TOODEEP   -15

/**
 * Constraint violation.  This status code is returned when constraints
 * defined the schema are violated.  These include XSD facets such as
 * min/maxOccurs, min/maxLength, patterns, etc..  Also ASN.1 value range,
 * size, and permitted alphabet constraints.
 */
#define RTERR_CONSVIO   -16

/**
 * Unexpected end-of-file error.  This status code is returned when an
 * unexpected end-of-file condition is detected on decode.  It is similar
 * to the ENDOFBUF error code described above except that in this case,
 * decoding is being done from a file stream instead of from a memory buffer.
 */
#define RTERR_ENDOFFILE -17

/**
 * Invalid UTF-8 character encoding.  This status code is returned by
 * the decoder when an invalid sequence of bytes is detected in a UTF-8
 * character string.
 */
#define RTERR_INVUTF8   -18

/**
 * Array index out-of-bounds.  This status code is returned when an
 * attempt is made to add something to an array and the given index is
 * outside the defined bounds of the array.
 */
#define RTERR_OUTOFBND  -19

/**
 * Invalid parameter passed to a function of method.  This status code is
 * returned by a function or method when it does an initial check on the
 * values of parameters passed in.  If a parameter is found to not have a
 * value in the expected range, this error code is returned.
 */
#define RTERR_INVPARAM  -20

/**
 * Invalid value format.  This status code is returned when a value is
 * received or passed into a function that is not in the expected format.
 * For example, the time string parsing function expects a string in the
 * form "nn:nn:nn" where n's are numbers.  If not in this format, this
 * error code is returned.
 */
#define RTERR_INVFORMAT -21

/**
 * Context not initialized.  This status code is returned when the
 * run-time context structure (OSCTXT) is attempted to be used without
 * having been initialized.  This can occur if rtxInitContext is not
 * invoked to initialize a context variable before use in any other API
 * call.  It can also occur is there is a license violation (for example,
 * evaluation license expired).
 */
#define RTERR_NOTINIT   -22

/**
 * Value will not fit in target variable.  This status is returned by
 * the decoder when a target variable is not large enough to hold a
 * a decoded value.  A typical case is an integer value that is too
 * large to fit in the standard C integer type (typically a 32-bit value)
 * on a given platform.  If this occurs, it is usually necessary to use
 * a configuration file setting to force the compiler to use a different
 * data type for the item.  For example, for integer, the \<isBigInteger/\>
 * setting can be used to force use of a big integer type.
 */
#define RTERR_TOOBIG    -23

/**
 * Invalid character.  This status code is returned when a character is
 * encountered that is not valid for a given data type.  For example,
 * if an integer value is being decoded and a non-numeric character is
 * encountered, this error will be raised.
 */
#define RTERR_INVCHAR   -24

/**
 * XML state error.  This status code is returned when the XML parser
 * is not in the correct state to do a certain operation.
 */
#define RTERR_XMLSTATE  -25

/**
 * XML parser error.  This status code in returned when the underlying
 * XML parser application (by default, this is Expat) returns an error
 * code.  The parser error code or text is returned as a parameter in
 * the errInfo structure within the context structure.
 */
#define RTERR_XMLPARSE  -26

/**
 * Sequence order error.  This status code is returned when decoding
 * an ASN.1 SEQUENCE or XSD xsd:sequence construct.  It is raised if
 * the elements were received in an order different than that specified
 * in the content model group definition.
 */
#define RTERR_SEQORDER  -27

/**
 * File not found.  This status code is returned if an attempt is made
 * to open a file input stream for decoding and the given file does not
 * exist.
 */
#define RTERR_FILNOTFOU -28

/**
 * Read error.  This status code if returned if a read I/O error is
 * encountered when reading from an input stream associated with a
 * physical device such as a file or socket.
 */
#define RTERR_READERR   -29

/**
 * Write error.  This status code if returned if a write I/O error is
 * encountered when attempting to output data to an output stream
 * associated with a physical device such as a file or socket.
 */
#define RTERR_WRITEERR  -30

/**
 * Invalid Base64 encoding.  This status code is returned when an
 * error is detected in decoding base64 data.
 */
#define RTERR_INVBASE64 -31

/**
 * Invalid socket.  This status code is returned when an attempt is made
 * to read or write from a scoket and the given socket handle is invalid.
 * This may be the result of not having established a proper connection
 * before trying to use the socket handle variable.
 */
#define RTERR_INVSOCKET -32

/**
 * Invalid attribute.  This status code is returned by the decoder when
 * an attribute is encountered in an XML instance that was not defined
 * in the XML schema.
 */
/* Parameters:
 * 1. C type name
 * 2. attribute name
 */
#define RTERR_INVATTR   -33

/**
 * Invalid regular expression.  This status code is returned when a
 * syntax error is detected in a regular expression value.  Details
 * of the syntax error can be obtained by invoking rtxErrPrint to
 * print the details of the error contained within the context
 * variable.
 */
#define RTERR_REGEXP    -34

/**
 * Pattern match error.  This status code is returned by the decoder
 * when a value in an XML instance does not match the pattern facet
 * defined in the XML schema.  It can also be returned by numeric encode
 * functions that cannot format a numeric value to match the pattern
 * specified for that value.
 */
#define RTERR_PATMATCH  -35

/**
 * Missing required attribute.  This status code is returned by the decoder
 * when an XML instance is missing a required attribute value as defined in
 * the XML schema.
 */
#define RTERR_ATTRMISRQ -36

/**
 * Host name could not be resolved.  This status code is returned from
 * run-time socket functions when they are unable to connect to a given
 * host computer.
 */
#define RTERR_HOSTNOTFOU -37

/**
 * HTTP protocol error.  This status code is returned by functions doing
 * HTTP protocol operations such as SOAP functions.  It is returned when
 * a protocol error is detected.  Details on the specific error can be
 * obtained by calling rtxErrPrint.
 */
#define RTERR_HTTPERR   -38

/**
 * SOAP error.  This status code when an error is detected when trying
 * to execute a SOAP operation.
 */
#define RTERR_SOAPERR   -39

/**
 * Evaluation license expired.  This error is returned from evaluation
 * versions of the run-time library when the hard-coded evaluation period
 * is expired.
 */
#define RTERR_EXPIRED   -40

/**
 * Unexpected element encountered.  This status code is returned when
 * an element is encountered in a position where something else (for
 * example, an attribute) was expected.
 */
#define RTERR_UNEXPELEM -41

/**
 * Invalid number of occurrences.  This status code is returned by the
 * decoder when an XML instance contains a number of occurrences of a
 * repeating element that is outside the bounds (minOccurs/maxOccurs)
 * defined for the element in the XML schema.
 */
#define RTERR_INVOCCUR  -42

/**
 * Invalid message buffer has been passed to decode or validate
 * method. This status code is returned by decode or validate method when
 * the used message buffer instance has type different from
 * OSMessageBufferIF::XMLDecode.
 */
#define RTERR_INVMSGBUF -43

/**
 * Element decode failed.  This status code and parameters are added to
 * the failure status by the decoder to allow the specific element on
 * which a decode error was detected to be identified.
 */
#define RTERR_DECELEMFAIL -44

/**
 * Attribute decode failed.  This status code and parameters are added to
 * the failure status by the decoder to allow the specific attribute on
 * which a decode error was detected to be identified.
 */
#define RTERR_DECATTRFAIL -45

/**
 * Stream in-use.  This status code is returned by stream functions
 * when an attempt is made to initialize a stream or create a reader or
 * writer when an existing stream is open in the context.  The existing
 * stream must first be closed before initializaing a stream for a new
 * operation.
 */
#define RTERR_STRMINUSE -46

/**
 * Null pointer.  This status code is returned when a null pointer
 * is encountered in a place where it is expected that the pointer
 * value is to be set.
 */
#define RTERR_NULLPTR -47

/**
 * General failure. Low level call returned error.
 */
#define RTERR_FAILED -48

/**
 * Attribute fixed value mismatch.  The attribute contained a value that
 * was different than the fixed value defined in the schema for the
 * attribute.
 */
/* Parameters:
 * 1. attribute name
 * 2. attribute value
 */
#define RTERR_ATTRFIXEDVAL -49

/**
 * Multiple errors occurred during an encode or decode operation.
 * See the error list within the context structure for a full list
 * of all errors.
 */
/* Parameters:
 * None
 */
#define RTERR_MULTIPLE -50

/**
 * This error is returned when decoding a derived type definition
 * and no information exists as to what type of data is in the
 * element content.  When decoding XML, this normally means that
 * an xsi:type attribute was not found identifying the type of content.
 */
/* Parameters:
 * None
 */
#define RTERR_NOTYPEINFO -51

/**
 * Address already in use.  This status code is returned when an attempt is
 * made to bind a socket to an address that is already in use.
 */
#define RTERR_ADDRINUSE -52

/**
 * Remote connection was reset.  This status code is returned when the
 * connection is reset by the remote host (via explicit command or a crash.
 */
#define RTERR_CONNRESET -53

/**
 * Network failure.  This status code is returned when the network or host
 * is down or otherwise unreachable.
 */
#define RTERR_UNREACHABLE -54

/**
 * Not connected.  This status code is returned when an operation is issued on
 * an unconnected socket.
 */
#define RTERR_NOCONN -55

/**
 * Connection refused.  This status code is returned when an attempt to
 * communicate on an open socket is refused by the host.
 */
#define RTERR_CONNREFUSED -56

/**
 * Invalid option.  This status code is returned when an invalid option is
 * passed to socket.
 */
#define RTERR_INVSOCKOPT -57

/**
 * This error is returned when decoded SOAP envelope is fault message
 */
/* Parameters:
 * None
 */
#define RTERR_SOAPFAULT -58

/**
 * This error is returned when an attempt is made to mark a stream
 * position on a stream type that does not support it.
 */
/* Parameters:
 * None
 */
#define RTERR_MARKNOTSUP -59

/**
 * Feature is not supported.  This status code is returned when a
 * feature that is currently not supported is encountered.  Support may
 * be added in a future release.
 */
/* Parameters:
 * 1. Brief description of not supported feature
 */
#define RTERR_NOTSUPP   -60     /* feature is not supported             */

/**
 * Unbalanced structure.  This error code is returned when parsing formatted
 * text such as XML or JSON and a block is not properly terminated.  For JSON,
 * this occurs when a '{' or '[' character does not a corresponding '}' or
 * ']' respectively.  For XML, it occurs when an open element does not have
 * a corresponding end element.
 */
#define RTERR_UNBAL     -61

/**
 * Expected name.  This error code is returned when parsing a name/value
 * pair and the name part is expected, but instead a value is encountered.
 */
#define RTERR_EXPNAME   -62

/**
 * Invalid Unicode sequence.  The sequence of characters received did not
 * comprise a valid unicode character.
 */
#define RTERR_UNICODE   -63

/**
 * Invalid boolean keyword.  This error code is returned when an invalid
 * boolean keyword in the format of the language being parsed is encountered.
 * For example, 'true' or 'false' in all lowercase letters may be all
 * that is acceptable.
 */
#define RTERR_INVBOOL   -64

/**
 * Invalid null keyword.  This error code is returned when an invalid
 * null keyword in the format of the language being parsed is encountered.
 * For example, 'null' in all lowercase letters may be all that is acceptable.
 */
#define RTERR_INVNULL   -65

/**
 * Invalid length.  This error code is returned when a length value is
 * parsed that is not consistent with other lengths in a message.
 * This typically happens when an inner length within a constructed type
 * is larger than the outer length value.
 */
#define RTERR_INVLEN    -66

/**
 * Unknown information element.  This error code is returned when an
 * unknown information element or extension is received and the
 * protocol specification indicates the element must be understood.
 */
#define RTERR_UNKNOWNIE -67

/**
 * Not aligned error.  This is returned when an element is expected to
 * start on a byte-aligned boundary and is found not to start on
 * an unaligned boundary.
 */
#define RTERR_NOTALIGNED -68

/**
 * Extraneous data.  This error is returned when after decoding is complete, 
 * additional undecoded data is still present in the message buffer.
 */
#define RTERR_EXTRDATA   -69

/**
 * @} rtxErrCodes
 */
#endif
