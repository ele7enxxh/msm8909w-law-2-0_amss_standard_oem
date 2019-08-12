/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                          PHONEBOOK MANAGER FUNCTIONS

GENERAL DESCRIPTION
  This file contains the routines to convert between the various coding
  schemes which PBM uses.   This file can translate between the following
  sets of characters.

    The frist is 16 bit Unicode (UCS2).
    The second is the legacy PBM 8 bit character set.
    The third is the GSM7 encoding used in SIM cards.
    The forth is the UCS2 encoding used in SIM cards.

  The SIM card encodings are used internally by PBM when encoding into
  a SIM.  Depending on the encoding schemes allowed, PBM will use the
  mechanism that can encode the most amount of characters.

  The PBM8 to UCS2 encoding schemes are used internally when we encounter
  any field with a PBM_FT_STRING associated with it.  PBM_FT_STRING's are 8
  bit encoded values.  PBM_FT_UCS2 are 16 bit encoded strings.  Clients of
  the phonebook manager must be ready for either type of string, however
  PBM will use the field_type to indicate what the client has received.

  The file has the following major interface functions:
      pbmconvert_init_conversion  - Must be called for all this to work.
      pbmconvert_ucs2_to_pbm8     - Converts UCS2 string to PBM8.
      pbmconvert_pbm8_to_ucs2     - Converts a PBM8 string to UCS2.
      pbmconvert_string_to_sim      - Encodes a UCS2 string for the SIM
      pbmconvert_sim_to_string      - Decodes a string from the SIM into UCS2.
  The remaining functions support this.

  Copyright (c) 2005-2009, 2014-2016 by QUALCOMM Technologies, Inc(QTI).
  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================
                        EDIT HISTORY FOR MODULE
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/pbm/src/pbm_conversion.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/24/16   sp      Logging improvements
05/05/16   dd      Adding support for special chinese character encoding
05/04/16   sp      Fix for extension characters in 0x81 encoding
09/07/15   stv     Fix for extension characters in 0x82 encoding
07/07/14   NR      Use v2.0 diag macros
05/30/14   NR      Fixing “out_base” argument calculation in function pbmconvert_parse_input () 
                   for 0x82 encoing
03/04/14   ram     Not possible to save SIM contact with name having some special 
                   characters that are not converted in GSM7
01/27/14   stv     Efficient conversion with usage of 0x81 UCS2 format rather extended GSM7 
07/10/09  ashwanik Support for CSIM and Dual Sim Features
09/29/08   sg      Stop processing a UCS2 string only when FF FF is read
10/10/06   cvs     Move messages to PBM's diag msg ID
05/03/06   cvs     Lint cleanup.
03/24/06   cvs     %x -> 0x%x in log messages
03/10/06   cvs     Lint fixes
02/23/06   cvs     Add string functions, reduce interface
02/09/06   cvs     When can't encode all characters on SIM, return errors
11/15/05   cvs     Lint fixes
08/22/05   AT      Moved contents inside of a FEATURE_PBM_TASK.
08/12/05   AT      Initial Revision
===========================================================================*/
#include "customer.h"
#include "uim_msg.h"
#include "comdef.h"
#include "stdlib.h"
#include "pbm.h"
#include "pbmlib.h"
#include "pbmutils.h"
#include "pbm_conversion.h"
#include "stringl/stringl.h"

/* Inside the SIM/USIM, the following coding techniques are allowed for the
 * name.
 */

/* A CONVERSION_TABLE is an array mapping 7 or 8 bit values to 16 bit values. */
typedef struct conversion_table
{
   uint8  octet;
   uint16 Unicode;
} CONVERSION_TABLE;



/* A PFCONVERT function changes a unicode string to the corresponding SIM
 * characters. */
typedef boolean (*PFCONVERT)( const uint16 in_str, uint8 **out_octets,
                              uint16 *out_len, uint16 out_base);

/* A SIM_CONVERSION function changes one or more SIM characters to a unicode
 * string */
typedef boolean (*SIM_CONVERSION)(uint8 **in_string, uint16 *in_len,
                                  uint16 **out_octets, uint16 ucs2_base);


/* The following tables maps the 256 characters of PBM8 to the corresponding
 * unicode characters. */
static const CONVERSION_TABLE  pbm8_to_unicode[] = {
{0x00, 0x0000}, /* Null */
{0x01, 0x0001}, /* Start of heading */
{0x02, 0x0002}, /* Start of text */
{0x03, 0x0003}, /* End of text */
{0x04, 0x0004}, /* End of transmission */
{0x05, 0x0005}, /* Inquiry */
{0x06, 0x0006}, /* ACK */
{0x07, 0x0007}, /* Bell */
{0x08, 0x0008}, /* Backspace */
{0x09, 0x0009}, /* Tab */
{0x0A, 0x000A}, /* Line feed */
{0x0B, 0x000B}, /* Vertical tab */
{0x0C, 0x000C}, /* Form feed */
{0x0D, 0x000D}, /* Carriage return */
{0x0E, 0x000E}, /* Shift out */
{0x0F, 0x000F}, /* Shift in */
{0x10, 0x0010}, /* Data link escape */
{0x11, 0x0011}, /* Device control one */
{0x12, 0x0012}, /* Device control two */
{0x13, 0x0013}, /* Device control three */
{0x14, 0x0014}, /* Device control four */
{0x15, 0x0015}, /* NAK */
{0x16, 0x0016}, /* Synch */
{0x17, 0x0017}, /* End of block */
{0x18, 0x0018}, /* Cancel */
{0x19, 0x0019}, /* End of medium */
{0x1A, 0x001A}, /* Substitute */
{0x1B, 0x001B}, /* ESC */
{0x1C, 0x001C}, /* File separator */
{0x1D, 0x001D}, /* Group separator */
{0x1E, 0x001E}, /* Record separator */
{0x1F, 0x001F}, /* Unit separator */
{0x20, 0x0020}, /* Space */
{0x21, 0x0021}, /* Exclamation mark */
{0x22, 0x0022}, /* Quotation mark */
{0x23, 0x0023}, /* Number sign */
{0x24, 0x0024}, /* Dollar sign */
{0x25, 0x0025}, /* Percent sign */
{0x26, 0x0026}, /* Ampersand */
{0x27, 0x0027}, /* Apostrophe */
{0x28, 0x0028}, /* Left parenthesis */
{0x29, 0x0029}, /* Right parenthesis */
{0x2A, 0x002A}, /* Asterisk */
{0x2B, 0x002B}, /* Plus */
{0x2C, 0x002C}, /* Comma */
{0x2D, 0x002D}, /* Hyphen */
{0x2E, 0x002E}, /* Full stop */
{0x2F, 0x002F}, /* Solidus */
{0x30, 0x0030}, /* Zero */
{0x31, 0x0031}, /* One */
{0x32, 0x0032}, /* Two */
{0x33, 0x0033}, /* Three */
{0x34, 0x0034}, /* Four */
{0x35, 0x0035}, /* Five */
{0x36, 0x0036}, /* Six */
{0x37, 0x0037}, /* Seven */
{0x38, 0x0038}, /* Eight */
{0x39, 0x0039}, /* Nine */
{0x3A, 0x003A}, /* Colon */
{0x3B, 0x003B}, /* Semicolon */
{0x3C, 0x003C}, /* < (Less than) */
{0x3D, 0x003D}, /* = Equals */
{0x3E, 0x003E}, /* > greater than */
{0x3F, 0x003F}, /* Question mark */
{0x40, 0x0040}, /* AT Sign */
{0x41, 0x0041}, /* Latin A */
{0x42, 0x0042}, /* Latin B */
{0x43, 0x0043}, /* Latin C */
{0x44, 0x0044}, /* Latin D */
{0x45, 0x0045}, /* Latin E */
{0x46, 0x0046}, /* Latin F */
{0x47, 0x0047}, /* Latin G */
{0x48, 0x0048}, /* Latin H */
{0x49, 0x0049}, /* Latin I */
{0x4A, 0x004A}, /* Latin J */
{0x4B, 0x004B}, /* Latin K */
{0x4C, 0x004C}, /* Latin L */
{0x4D, 0x004D}, /* Latin M */
{0x4E, 0x004E}, /* Latin N */
{0x4F, 0x004F}, /* Latin O */
{0x50, 0x0050}, /* Latin P */
{0x51, 0x0051}, /* Latin Q */
{0x52, 0x0052}, /* Latin R */
{0x53, 0x0053}, /* Latin S */
{0x54, 0x0054}, /* Latin T */
{0x55, 0x0055}, /* Latin U */
{0x56, 0x0056}, /* Latin V */
{0x57, 0x0057}, /* Latin W */
{0x58, 0x0058}, /* Latin X */
{0x59, 0x0059}, /* Latin Y */
{0x5A, 0x005A}, /* Latin Z */
{0x5B, 0x005B}, /* Left square bracket */
{0x5C, 0x005C}, /* Reverse solidus */
{0x5D, 0x005D}, /* Right square bracket */
{0x5E, 0x005E}, /* Circumflex accent */
{0x5F, 0x005F}, /* Low line */
{0x60, 0x0060}, /* Grave accent */
{0x61, 0x0061}, /* Latin a */
{0x62, 0x0062}, /* Latin b */
{0x63, 0x0063}, /* Latin c */
{0x64, 0x0064}, /* Latin d */
{0x65, 0x0065}, /* Latin e */
{0x66, 0x0066}, /* Latin f */
{0x67, 0x0067}, /* Latin g */
{0x68, 0x0068}, /* Latin h */
{0x69, 0x0069}, /* Latin i */
{0x6A, 0x006A}, /* Latin j */
{0x6B, 0x006B}, /* Latin k */
{0x6C, 0x006C}, /* Latin l */
{0x6D, 0x006D}, /* Latin m */
{0x6E, 0x006E}, /* Latin n */
{0x6F, 0x006F}, /* Latin o */
{0x70, 0x0070}, /* Latin p */
{0x71, 0x0071}, /* Latin q */
{0x72, 0x0072}, /* Latin r */
{0x73, 0x0073}, /* Latin s */
{0x74, 0x0074}, /* Latin t */
{0x75, 0x0075}, /* Latin u */
{0x76, 0x0076}, /* Latin v */
{0x77, 0x0077}, /* Latin w */
{0x78, 0x0078}, /* Latin x */
{0x79, 0x0079}, /* Latin y */
{0x7A, 0x007A}, /* Latin z */
{0x7B, 0x007B}, /* Left curly bracket */
{0x7C, 0x007C}, /* Vertical line */
{0x7D, 0x007D}, /* Right curly bracket */
{0x7E, 0x007E}, /* Tilde */
{0x7F, 0x007F}, /* DEL */
{0x80, 0x0080}, /* control character 0x80 */
{0x81, 0x0081}, /* control character 0x81 */
{0x82, 0x0082}, /* control character 0x82 */
{0x83, 0x0083}, /* control character 0x83 */
{0x84, 0x0084}, /* control character 0x84 */
{0x85, 0x0085}, /* control character 0x85 */
{0x86, 0x0086}, /* control character 0x86 */
{0x87, 0x0087}, /* control character 0x87 */
{0x88, 0x0088}, /* control character 0x88 */
{0x89, 0x0089}, /* control character 0x89 */
{0x8A, 0x008A}, /* control character 0x8A */
{0x8B, 0x008B}, /* control character 0x8B */
{0x8C, 0x008C}, /* control character 0x8C */
{0x8D, 0x008D}, /* control character 0x8D */
{0x8E, 0x008E}, /* control character 0x8E */
{0x8F, 0x008F}, /* control character 0x8F */
{0x90, 0x0394}, /* Greek capital letter delta */
{0x91, 0x20AC}, /* Euro sign */
{0x92, 0x03A6}, /* Greek capital letter phi */
{0x93, 0x0393}, /* Greek capital letter gamma */
{0x94, 0x039B}, /* Greek capital letter lamda */
{0x95, 0x03A9}, /* Greek capital letter omega */
{0x96, 0x03A0}, /* Greek capital letter pi */
{0x97, 0x03A8}, /* Greek capital letter psi */
{0x98, 0x03A3}, /* Greek capital letter sigma */
{0x99, 0x0398}, /* Greek capital letter theta */
{0x9A, 0x039E}, /* Greek capital letter xi */
{0x9B, 0x009B}, /* control character 0x9B */
{0x9C, 0x009C}, /* control character 0x9C */
{0x9D, 0x009D}, /* control character 0x9D */
{0x9E, 0x009E}, /* control character 0x9E */
{0x9F, 0x009F}, /* control character 0x9F */
{0xA0, 0x00A0}, /* Non-Block Space */
{0xA1, 0x00A1}, /* Inverted exclamation mark */
{0xA2, 0x00A2}, /* Cent sign */
{0xA3, 0x00A3}, /* Pound sign */
{0xA4, 0x00A4}, /* Currency sign */
{0xA5, 0x00A5}, /* Yen sign */
{0xA6, 0x00A6}, /* Broken Vertical bar */
{0xA7, 0x00A7}, /* Section sign */
{0xA8, 0x00A8}, /* Diaeresis */
{0xA9, 0x00A9}, /* Copyright sign */
{0xAA, 0x00AA}, /* Feminine ordinal indicator */
{0xAB, 0x00AB}, /* Left-pointing double angle quotation mark */
{0xAC, 0x00AC}, /* Not sign */
{0xAD, 0x00AD}, /* Soft hyphen */
{0xAE, 0x00AE}, /* Registered sign */
{0xAF, 0x00AF}, /* Macron */
{0xB0, 0x00B0}, /* Degree sign */
{0xB1, 0x00B1}, /* Plus-minus sign */
{0xB2, 0x00B2}, /* Superscript two */
{0xB3, 0x00B3}, /* Superscript three */
{0xB4, 0x00B4}, /* Acute accent */
{0xB5, 0x00B5}, /* Micro sign */
{0xB6, 0x00B6}, /* Pilcrow sign */
{0xB7, 0x00B7}, /* Middle dot */
{0xB8, 0x00B8}, /* Cedilla */
{0xB9, 0x00B9}, /* Superscript one */
{0xBA, 0x00BA}, /* Masculine ordinal indicator */
{0xBB, 0x00BB}, /* Right-pointing double angle quotation mark */
{0xBC, 0x00BC}, /* Fraction one quarter */
{0xBD, 0x00BD}, /* Fraction one half */
{0xBE, 0x00BE}, /* Fraction three quarters */
{0xBF, 0x00BF}, /* Inverted question mark */
{0xC0, 0x00C0}, /* Latin A With grave */
{0xC1, 0x00C1}, /* Latin A With acute */
{0xC2, 0x00C2}, /* Latin A With circumflex */
{0xC3, 0x00C3}, /* Latin A With tilde */
{0xC4, 0x00C4}, /* Latin A With diaeresis */
{0xC5, 0x00C5}, /* Latin A With ring above */
{0xC6, 0x00C6}, /* Latin AE */
{0xC7, 0x00C7}, /* Latin C with cedilla */
{0xC8, 0x00C8}, /* Latin E with grave */
{0xC9, 0x00C9}, /* Latin E with acute */
{0xCA, 0x00CA}, /* Latin E with circumflex */
{0xCB, 0x00CB}, /* Latin E with diaeresis */
{0xCC, 0x00CC}, /* Latin I with grave */
{0xCD, 0x00CD}, /* Latin I with acute */
{0xCE, 0x00CE}, /* Latin I with circumflex */
{0xCF, 0x00CF}, /* Latin I with diaeresis */
{0xD0, 0x00D0}, /* Latin CAPITAL LETTER ETH (Icelandic) */
{0xD1, 0x00D1}, /* Latin CAPITAL LETTER N WITH TILDE */
{0xD2, 0x00D2}, /* Latin CAPITAL LETTER O WITH GRAVE */
{0xD3, 0x00D3}, /* Latin CAPITAL LETTER O WITH ACUTE */
{0xD4, 0x00D4}, /* Latin CAPITAL LETTER O WITH CIRCUMFLEX */
{0xD5, 0x00D5}, /* Latin CAPITAL LETTER O WITH TILDE */
{0xD6, 0x00D6}, /* Latin CAPITAL LETTER O WITH DIAERESIS */
{0xD7, 0x00D7}, /* MULTIPLICATION SIGN */
{0xD8, 0x00D8}, /* Latin CAPITAL LETTER O WITH STROKE */
{0xD9, 0x00D9}, /* Latin CAPITAL LETTER U WITH GRAVE */
{0xDA, 0x00DA}, /* Latin CAPITAL LETTER U WITH ACUTE */
{0xDB, 0x00DB}, /* Latin CAPITAL LETTER U WITH CIRCUMFLEX */
{0xDC, 0x00DC}, /* Latin CAPITAL LETTER U WITH DIAERESIS */
{0xDD, 0x00DD}, /* Latin CAPITAL LETTER Y WITH ACUTE */
{0xDE, 0x00DE}, /* Latin CAPITAL LETTER THORN (Icelandic) */
{0xDF, 0x00DF}, /* Latin SHARP S (German) */
{0xE0, 0x00E0}, /* Latin A WITH GRAVE */
{0xE1, 0x00E1}, /* Latin A WITH ACUTE */
{0xE2, 0x00E2}, /* Latin A WITH CIRCUMFLEX */
{0xE3, 0x00E3}, /* Latin A WITH TILDE */
{0xE4, 0x00E4}, /* Latin A WITH DIAERESIS */
{0xE5, 0x00E5}, /* Latin A WITH RING ABOVE */
{0xE6, 0x00E6}, /* Latin AE */
{0xE7, 0x00E7}, /* Latin C WITH CEDILLA */
{0xE8, 0x00E8}, /* Latin E WITH GRAVE */
{0xE9, 0x00E9}, /* Latin E WITH ACUTE */
{0xEA, 0x00EA}, /* Latin E WITH CIRCUMFLEX */
{0xEB, 0x00EB}, /* Latin E WITH DIAERESIS */
{0xEC, 0x00EC}, /* Latin I WITH GRAVE */
{0xED, 0x00ED}, /* Latin I WITH ACUTE */
{0xEE, 0x00EE}, /* Latin I WITH CIRCUMFLEX */
{0xEF, 0x00EF}, /* Latin I WITH DIAERESIS */
{0xF0, 0x00F0}, /* Latin ETH (Icelandic) */
{0xF1, 0x00F1}, /* Latin N WITH TILDE */
{0xF2, 0x00F2}, /* Latin O WITH GRAVE */
{0xF3, 0x00F3}, /* Latin O WITH ACUTE */
{0xF4, 0x00F4}, /* Latin O WITH CIRCUMFLEX */
{0xF5, 0x00F5}, /* Latin O WITH TILDE */
{0xF6, 0x00F6}, /* Latin O WITH DIAERESIS */
{0xF7, 0x00F7}, /* DIVISION SIGN */
{0xF8, 0x00F8}, /* Latin O WITH STROKE */
{0xF9, 0x00F9}, /* Latin U WITH GRAVE */
{0xFA, 0x00FA}, /* Latin U WITH ACUTE */
{0xFB, 0x00FB}, /* Latin U WITH CIRCUMFLEX */
{0xFC, 0x00FC}, /* Latin U WITH DIAERESIS */
{0xFD, 0x00FD}, /* Latin Y WITH ACUTE */
{0xFE, 0x00FE}, /* Latin THORN (Icelandic) */
{0xFF, 0x00FF}, /* Latin Y WITH DIAERESIS */
};

/* The following tables maps the corresponding unicode characters back
 * to the 256 characters of PBM8.  This table is built at initialization.
 */
static CONVERSION_TABLE  unicode_to_pbm8[ARR_SIZE(pbm8_to_unicode)];

static int pbmconvert_unicode_compare(const void *first_arg,
                                      const void *second_arg);

/* The following table maps the gsm7 alphabet to the corresponding unicode
 * characters. This table is exactly 128 entries large. */
static const CONVERSION_TABLE  gsm7_to_unicode[] = {
{0x00, 0x0040}, /* COMMERCIAL AT */
{0x01, 0x00A3}, /* POUND SIGN */
{0x02, 0x0024}, /* DOLLAR SIGN */
{0x03, 0x00A5}, /* YEN SIGN */
{0x04, 0x00E8}, /* Latin E WITH GRAVE */
{0x05, 0x00E9}, /* Latin E WITH ACUTE */
{0x06, 0x00F9}, /* Latin U WITH GRAVE */
{0x07, 0x00EC}, /* Latin I WITH GRAVE */
{0x08, 0x00F2}, /* Latin O WITH GRAVE */
{0x09, 0x00E7}, /* Latin C WITH CEDILLA */
{0x0A, 0x000A}, /* LINE FEED */
{0x0B, 0x00D8}, /* Latin CAPITAL LETTER O WITH STROKE */
{0x0C, 0x00F8}, /* Latin O WITH STROKE */
{0x0D, 0x000D}, /* CARRIAGE RETURN */
{0x0E, 0x00C5}, /* Latin CAPITAL LETTER A WITH RING ABOVE */
{0x0F, 0x00E5}, /* Latin A WITH RING ABOVE */
{0x10, 0x0394}, /* GREEK CAPITAL LETTER DELTA */
{0x11, 0x005F}, /* LOW LINE */
{0x12, 0x03A6}, /* GREEK CAPITAL LETTER PHI */
{0x13, 0x0393}, /* GREEK CAPITAL LETTER GAMMA */
{0x14, 0x039B}, /* GREEK CAPITAL LETTER LAMDA */
{0x15, 0x03A9}, /* GREEK CAPITAL LETTER OMEGA */
{0x16, 0x03A0}, /* GREEK CAPITAL LETTER PI */
{0x17, 0x03A8}, /* GREEK CAPITAL LETTER PSI */
{0x18, 0x03A3}, /* GREEK CAPITAL LETTER SIGMA */
{0x19, 0x0398}, /* GREEK CAPITAL LETTER THETA */
{0x1A, 0x039E}, /* GREEK CAPITAL LETTER XI */
{0x1B, 0x00A0}, /* ESCAPE TO EXTENSION TABLE */
{0x1C, 0x00C6}, /* Latin CAPITAL LETTER AE */
{0x1D, 0x00E6}, /* Latin AE */
{0x1E, 0x00DF}, /* Latin SHARP S (German) */
{0x1F, 0x00C9}, /* Latin CAPITAL LETTER E WITH ACUTE */
{0x20, 0x0020}, /* SPACE */
{0x21, 0x0021}, /* EXCLAMATION MARK */
{0x22, 0x0022}, /* QUOTATION MARK */
{0x23, 0x0023}, /* NUMBER SIGN */
{0x24, 0x00A4}, /* CURRENCY SIGN */
{0x25, 0x0025}, /* PERCENT SIGN */
{0x26, 0x0026}, /* AMPERSAND */
{0x27, 0x0027}, /* APOSTROPHE */
{0x28, 0x0028}, /* LEFT PARENTHESIS */
{0x29, 0x0029}, /* RIGHT PARENTHESIS */
{0x2A, 0x002A}, /* ASTERISK */
{0x2B, 0x002B}, /* PLUS SIGN */
{0x2C, 0x002C}, /* COMMA */
{0x2D, 0x002D}, /* HYPHEN-MINUS */
{0x2E, 0x002E}, /* FULL STOP */
{0x2F, 0x002F}, /* SOLIDUS */
{0x30, 0x0030}, /* DIGIT ZERO */
{0x31, 0x0031}, /* DIGIT ONE */
{0x32, 0x0032}, /* DIGIT TWO */
{0x33, 0x0033}, /* DIGIT THREE */
{0x34, 0x0034}, /* DIGIT FOUR */
{0x35, 0x0035}, /* DIGIT FIVE */
{0x36, 0x0036}, /* DIGIT SIX */
{0x37, 0x0037}, /* DIGIT SEVEN */
{0x38, 0x0038}, /* DIGIT EIGHT */
{0x39, 0x0039}, /* DIGIT NINE */
{0x3A, 0x003A}, /* COLON */
{0x3B, 0x003B}, /* SEMICOLON */
{0x3C, 0x003C}, /* LESS-THAN SIGN */
{0x3D, 0x003D}, /* EQUALS SIGN */
{0x3E, 0x003E}, /* GREATER-THAN SIGN */
{0x3F, 0x003F}, /* QUESTION MARK */
{0x40, 0x00A1}, /* INVERTED EXCLAMATION MARK */
{0x41, 0x0041}, /* Latin CAPITAL LETTER A */
{0x42, 0x0042}, /* Latin CAPITAL LETTER B */
{0x43, 0x0043}, /* Latin CAPITAL LETTER C */
{0x44, 0x0044}, /* Latin CAPITAL LETTER D */
{0x45, 0x0045}, /* Latin CAPITAL LETTER E */
{0x46, 0x0046}, /* Latin CAPITAL LETTER F */
{0x47, 0x0047}, /* Latin CAPITAL LETTER G */
{0x48, 0x0048}, /* Latin CAPITAL LETTER H */
{0x49, 0x0049}, /* Latin CAPITAL LETTER I */
{0x4A, 0x004A}, /* Latin CAPITAL LETTER J */
{0x4B, 0x004B}, /* Latin CAPITAL LETTER K */
{0x4C, 0x004C}, /* Latin CAPITAL LETTER L */
{0x4D, 0x004D}, /* Latin CAPITAL LETTER M */
{0x4E, 0x004E}, /* Latin CAPITAL LETTER N */
{0x4F, 0x004F}, /* Latin CAPITAL LETTER O */
{0x50, 0x0050}, /* Latin CAPITAL LETTER P */
{0x51, 0x0051}, /* Latin CAPITAL LETTER Q */
{0x52, 0x0052}, /* Latin CAPITAL LETTER R */
{0x53, 0x0053}, /* Latin CAPITAL LETTER S */
{0x54, 0x0054}, /* Latin CAPITAL LETTER T */
{0x55, 0x0055}, /* Latin CAPITAL LETTER U */
{0x56, 0x0056}, /* Latin CAPITAL LETTER V */
{0x57, 0x0057}, /* Latin CAPITAL LETTER W */
{0x58, 0x0058}, /* Latin CAPITAL LETTER X */
{0x59, 0x0059}, /* Latin CAPITAL LETTER Y */
{0x5A, 0x005A}, /* Latin CAPITAL LETTER Z */
{0x5B, 0x00C4}, /* Latin CAPITAL LETTER A WITH DIAERESIS */
{0x5C, 0x00D6}, /* Latin CAPITAL LETTER O WITH DIAERESIS */
{0x5D, 0x00D1}, /* Latin CAPITAL LETTER N WITH TILDE */
{0x5E, 0x00DC}, /* Latin CAPITAL LETTER U WITH DIAERESIS */
{0x5F, 0x00A7}, /* SECTION SIGN */
{0x60, 0x00BF}, /* INVERTED QUESTION MARK */
{0x61, 0x0061}, /* Latin A */
{0x62, 0x0062}, /* Latin B */
{0x63, 0x0063}, /* Latin C */
{0x64, 0x0064}, /* Latin D */
{0x65, 0x0065}, /* Latin E */
{0x66, 0x0066}, /* Latin F */
{0x67, 0x0067}, /* Latin G */
{0x68, 0x0068}, /* Latin H */
{0x69, 0x0069}, /* Latin I */
{0x6A, 0x006A}, /* Latin J */
{0x6B, 0x006B}, /* Latin K */
{0x6C, 0x006C}, /* Latin L */
{0x6D, 0x006D}, /* Latin M */
{0x6E, 0x006E}, /* Latin N */
{0x6F, 0x006F}, /* Latin O */
{0x70, 0x0070}, /* Latin P */
{0x71, 0x0071}, /* Latin Q */
{0x72, 0x0072}, /* Latin R */
{0x73, 0x0073}, /* Latin S */
{0x74, 0x0074}, /* Latin T */
{0x75, 0x0075}, /* Latin U */
{0x76, 0x0076}, /* Latin V */
{0x77, 0x0077}, /* Latin W */
{0x78, 0x0078}, /* Latin X */
{0x79, 0x0079}, /* Latin Y */
{0x7A, 0x007A}, /* Latin Z */
{0x7B, 0x00E4}, /* Latin A WITH DIAERESIS */
{0x7C, 0x00F6}, /* Latin O WITH DIAERESIS */
{0x7D, 0x00F1}, /* Latin N WITH TILDE */
{0x7E, 0x00FC}, /* Latin U WITH DIAERESIS */
{0x7F, 0x00E0}, /* Latin A WITH GRAVE */
};
/* Unicode to gsm7 maps the Unicode characters to the GSM 7
 * alphabet.  It is built at runtime from the gsm7_to_unicode table
 * above. */
static CONVERSION_TABLE  unicode_to_gsm7[ARR_SIZE(gsm7_to_unicode)];

/* If the following characters are present after a 0x1B, then
 * this character is used.  Otherwise, the combination "0x00A0"
 * and this character orred with 0x7F is used. The only character
 * that only appears in this table is the EURO sign.  Thus, if
 * we are writing an entry, this is the only character that will
 * use the extension record.  However, reading a UIM written elsewhere
 * may have the other sequences.  Since EURO is the most likely one
 * for us to read (assuming we wrote it), it appears first.
 */
static const CONVERSION_TABLE  gsm7ext_to_unicode[] = {
{0x65, 0x20AC}, /* EURO SIGN */
{0x0A, 0x000C}, /* FORM FEED */
{0x14, 0x005E}, /* CIRCUMFLEX ACCENT */
{0x28, 0x007B}, /* LEFT CURLY BRACKET */
{0x29, 0x007D}, /* RIGHT CURLY BRACKET */
{0x2F, 0x005C}, /* REVERSE SOLIDUS */
{0x3C, 0x005B}, /* LEFT SQUARE BRACKET */
{0x3D, 0x007E}, /* TILDE */
{0x3E, 0x005D}, /* RIGHT SQUARE BRACKET */
{0x40, 0x007C}, /* VERTICAL LINE */
};


static boolean find_gsm_ext(uint8 second_octet, uint16 *new_value);

static void convert_gsm7_to_unicode(uint8 gsm7_char, uint16 *ucs2_char);




static boolean pbmconvert_convert_ucs2_to_ucs2_80( uint16 in_ucs2,
                                                   uint8 **out_octets,
                                                   uint16 *out_len,
                                                   uint16 out_base);

static boolean pbmconvert_convert_ucs2_to_ucs2_81 (uint16 in_ucs2,
                                                   uint8 **out_octets,
                                                   uint16 *out_len,
                                                   uint16 out_base);

static boolean pbmconvert_convert_ucs2_to_ucs2_82 (uint16 in_ucs2,
                                                   uint8 **out_octets,
                                                   uint16 *out_len,
                                                   uint16 out_base);

static boolean pbmconvert_convert_ucs2_to_gsm7    (uint16 in_ucs2,
                                                   uint8 **out_octets,
                                                   uint16 *out_len,
                                                   uint16 out_base);


static uint16 pbmconvert_pbm8_source(const void **in_ptr);

static boolean pbmconvert_convert_one_ucs2_to_pbm8(uint16 ucs2_char,
                                                   uint8 *pbm8_ptr);


static boolean pbmconvert_convert_one_ucs2_to_gsm7(uint16 ucs2_char,
                                                   uint8 *gsm7_ptr,
                                                   uint16 *gsm7_len);

static boolean pbmconvert_sim_ucs2_to_ucs2(uint8 **in_string,
                                           uint16 *in_len,
                                           uint16 **out_octets,
                                           uint16 ucs2_base);

static boolean pbmconvert_sim_gsm7_to_ucs2(uint8 **in_string, uint16 *in_len,
                                           uint16 **out_octets,
                                           uint16 ucs2_base);

static boolean pbmconvert_sim_ucs2_halfpage_to_ucs2(uint8 **in_string,
                                                    uint16 *in_len,
                                                    uint16 **out_octets,
                                                    uint16 ucs2_base);

/*===========================================================================
FUNCTION pbmconvert_string_to_sim

DESCRIPTION
  Convert string to SIM encoding

  in_str:       [in]  - String to be converted
  in_len:       [in]  - Length of said string.
  in_type:      [in]  - Whether the string is PBM8 or UCS2.
  out_octets:   [out] - Resulting SIM characters are written here.
  out_len:      [in]  - Size of the resulting output buffer.
  out_converts: [out] - The number of characters that got converted
  gsm7_only:    [in]  - TRUE if we must encode using GSM7 alphabet.

  This function takes an input string and translates it to the correct
  output format for the SIM.  The "brains" of this function lie in
  the pbmconvert_parse_input function which follows this one.

DEPENDENCIES
  None.

RETURN VALUE
  Returns number of octets used in the resulting string.

SIDE EFFECTS
  None.
===========================================================================*/
int pbmconvert_string_to_sim (const void *in_str, uint16 in_len,
                              pbm_field_type_e_type in_type,
                              uint8 *out_octets, uint16 out_len,
                              uint16 *out_converts, boolean gsm7_only)
{
   sim_coding_mode_enum_type out_mode = SIM_CODING_GSM7;
   PFCONVERT convert_fn;
   uint16 out_base;
   boolean done;
   pfsource_fn src_fn;
   uint16  output_size;

   PBM_CHECK_PTR2_RET(out_octets, out_converts, 0);

   // The input type determines which function is used to read in_str.
   if (in_type == PBM_FT_STRING)
   {
      src_fn = pbmconvert_pbm8_source;
   } else
   {
      src_fn = pbmconvert_return_input_string_first_ucs2_char;
   }

   /* Parse the in_str characters, looking for the best coding technique. */
   /* This function also tells us how big the result will be.             */
   pbmconvert_parse_input(in_str, in_len, &out_mode, &out_len, &out_base,
                          out_converts, src_fn, gsm7_only);
   output_size = out_len;    /* We will return what was given to us here. */

   /* If the coding is not GSM7, we need to write the header bytes first. */
   switch (out_mode)
   {
      case SIM_CODING_UCS2_HALF_PAGE_1:
         /* The header consists of 0x81, Number of chars, One octet of base. */
         *out_octets++ = (uint8) out_mode;
         *out_octets++ = (out_len-UCS81_LEN_OH) & 0xFF;
         *out_octets++ = (uint8) (out_base >> 7) & 0x00FF;
         out_len -= UCS81_LEN_OH;
         convert_fn = pbmconvert_convert_ucs2_to_ucs2_81; // use 0x81 type.
         break;
      case SIM_CODING_UCS2_HALF_PAGE_2:
         /* The header consists of 0x82, Number of chars, Two octets of base. */
         *out_octets++ = (uint8) out_mode;
         *out_octets++ = (out_len - UCS82_LEN_OH) & 0xFF;
         *out_octets++ = (uint8) (out_base >> 8) & 0x00FF;
         *out_octets++ = (uint8) (out_base) & 0x00FF;
         convert_fn = pbmconvert_convert_ucs2_to_ucs2_82;  // use 0x82 type
         out_len -= UCS82_LEN_OH;
         break;
      case SIM_CODING_PURE_UCS2:
         /* The header consists of 0x80, and then two octets per char. */
         *out_octets++ = (uint8) out_mode;
         out_len -= UCS80_LEN_OH;
         convert_fn = pbmconvert_convert_ucs2_to_ucs2_80;  // use 0x80 type
         break;
      default:
         /* No header, we will use GSM7 for as long as we can. */
         convert_fn = pbmconvert_convert_ucs2_to_gsm7; // use GSM7 type
         break;
   }

   do
   {
      /* Convert the in_str, one item at a time, until the convert fn indicates
       * we are done.  */
      done = (*convert_fn)((*src_fn)(&in_str), &out_octets, &out_len, out_base);
   } while (!done);

   return(output_size);
}


/*===========================================================================
FUNCTION pbmconvert_parse_input

DESCRIPTION
  Parse input string and determine the best SIM encoding to use

  in_str:  [in] Either PBM8 or UCS2 string to store.
  in_len:  [in] Length of the character array.

  out_mode:    [out]  The mode we will use to encode it.
  out_len:  [in/out]  in = Number of octets we can store.
                     out = The number of octets we will need.
  out_base:    [out]  The base if using 81 or 82 coding schemes.
  out_converts:[out] The number of characters that got converted
  src_fn:       [in]  Function that converts input to Unicode.
  gsm7_only:    [in]  True if we must encode using GSM7 alphabet.

  This function looks at the input string, and determines which of
  the encoding schemes would encode the most characters of the input
  string.  To do this, it attempts to convert it to each of the four
  different mechanisms, stopping when it hits something illegal.  If
  gsm7_only is set, we know the other 3 encodings are illegal, thus
  we encode as much as we can.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void pbmconvert_parse_input(
  const void                *in_str,
  uint16                     in_len,
  sim_coding_mode_enum_type *out_mode,
  uint16                    *out_len,
  uint16                    *out_base,
  uint16                    *out_converts,
  pfsource_fn                src_fn,
  boolean                    gsm7_only
)
{
  int                     index                 = 0;             /* Loop variable over input string.*/
  uint16                  ucs2_char             = 0;             /* Unicode character we are converting.*/
  uint16                  output_size           = 0;             /* Octets possible to use.*/
  uint16                  curr_gsm7_length      = 0;             /* Current Unicode needs how many GSM7 octets */
  uint8                   gsm7_chars[3];                         /* Where Current GSM7 octets are placed.*/

  uint16                  ucs2_pattern1         = 0;             /* When encoding with format 0x81, what is base? */
  uint16                  ucs2_pattern2_low     = 0;             /* When encoding with format 0x82, what is the */
  uint16                  ucs2_pattern2_high    = 0;             /* range of bases.*/

  boolean                 all_gsm7              = TRUE;          /* Indicates all characters seen are GSM7 */

  boolean                 use_pattern1          = TRUE;          /* Indicates we can still use format 0x81.*/
  boolean                 pattern1_set          = FALSE;         /* Indicates we set a pattern for format 0x81 */

  boolean                 use_pattern2          = TRUE;          /* Indicates we can still use format 0x82.*/
  boolean                 pattern2_set          = FALSE;         /* Indicates we set a pattern for format 0x82 */

  uint16                  gsm7_len              = GSM7_LEN_OH;   /* Number of octets if GSM7 encoding used.*/
  uint16                  ucs80_len             = UCS80_LEN_OH;  /* Number of octets if 0x80 format used.*/
  uint16                  ucs81_len             = UCS81_LEN_OH;  /* Number of octets if 0x81 format used.*/
  uint16                  ucs82_len             = UCS82_LEN_OH;  /* Number of octets if 0x82 format used.*/

  uint16                  gsm7_converts         = 0;             /* Number of src characters GSM7 could encode */
  uint16                  ucs80_converts        = 0;             /* Number of src characters 0x80 could encode */
  uint16                  ucs81_converts        = 0;             /* Number of src characters 0x81 could encode */
  uint16                  ucs82_converts        = 0;             /* Number of src characters 0x82 could encode */

  uint16                  best_converts         = 0;             /* To determine which format to use, save best.*/

  /* Make sure no NULL pointers are given. */
  PBM_CHECK_PTR4_RET(in_str, out_mode, out_len, out_base, VOID);
  PBM_CHECK_PTR2_RET(src_fn, out_converts, VOID);

  /* Save the output size for convenience. */
  output_size = *out_len;

  // For every character of input, while we can do something on output,
  // examine the character.
  for (index = 0;
      ((index < in_len) &&
       (all_gsm7 || use_pattern1 || use_pattern2 ||
       (ucs80_len < output_size)));
       index++)
  {
    ucs2_char = (*src_fn)(&in_str);

    /* First determine if its GSM7.  This is useful for GSM7 encoding,
       as well as 0x81 and 0x82 formats. */
    curr_gsm7_length = sizeof(gsm7_chars);
    if (pbmconvert_convert_one_ucs2_to_gsm7(ucs2_char,
                                            gsm7_chars,
                                            &curr_gsm7_length))
    {
      /* It can be converted to GSM7, if we are still trying GSM7,
         save the fact that we can do this.*/
      if (all_gsm7 || gsm7_only)
      {
        gsm7_len += curr_gsm7_length;
        gsm7_converts++;
      }
    }
    else
    {
      /* Not GSM7, we stop trying to save more GSM7 with this one.*/
      all_gsm7 = FALSE;
    }

    /* if gsm7_only is TRUE, we can't even try the other UCS encodings */
    if (!gsm7_only)
    {
      /* If we are still trying convert it to the 0x81 format. */
      if (use_pattern1 && ucs81_len < output_size)
      {
        /* The 0x81 format requires the highest bit to be off */
        if (ucs2_char & 0x8000)
        {
          if (curr_gsm7_length == 0)
          {
            use_pattern1 = FALSE;
          }
        }
        else
        {
          /* If we haven't tried yet, this pattern works. */
          if (!pattern1_set)
          {
            ucs2_pattern1 = (ucs2_char & 0x7F80);
            /*This need to be started for characters not converted in GSM7*/
            if(ucs2_pattern1)
            {
              pattern1_set = TRUE;
            }
            
            if (curr_gsm7_length == 0)
            {
              ucs81_len++;
            }
            else
            {
              ucs81_len += curr_gsm7_length;
            }
          }
          else
          {
            /* Do all characters meet GSM7 or the
               bit pattern: 0hhh hhhh hXXX XXXX */
            if (ucs2_pattern1 != (ucs2_char & 0x7F80))
            {
              /* check if possible with gsm7 */
              if(curr_gsm7_length == 0)
              {
                use_pattern1 = FALSE;
                /* not possible with gsm7,leave it*/
              }
              else
              {
                ucs81_len += curr_gsm7_length;
              }
            }
            else
            {
              ucs81_len++;
            }
          }
        }

        /* If we are still trying pattern1, this character can be encoded. */
        if (use_pattern1)
        {
          ucs81_converts++;
        }
      }

      /* If we are still trying convert it to the 0x82 format. */
      if (use_pattern2 && ucs82_len < output_size)
      {
        /* First non-GSM7 character? */
        if (!pattern2_set)
        {
          /*This need to be started for characters not converted in GSM7*/
          if(curr_gsm7_length == 0)
          {
            pattern2_set       = TRUE;
            ucs2_pattern2_low  = ucs2_char;
            ucs2_pattern2_high = ucs2_char;
            ucs82_len++;
          }
          else
          {
            ucs82_len += curr_gsm7_length;
          }
        }
        else
        {
          /* If it is not a GSM convertable don't change ucs2_pattern2_low 
             and ucs2_pattern2_high as it will give wrong out_base,Results in 
             Trucation of characters written to the card */
          if (curr_gsm7_length == 0)
          {
            /* Keep the highest and lowest, if they are now out of
               range, we can't use 0x82 format.*/
            if (ucs2_char < ucs2_pattern2_low)
            {
              ucs2_pattern2_low = ucs2_char;
            }
            else if (ucs2_char > ucs2_pattern2_high)
            {
              ucs2_pattern2_high = ucs2_char;
            }
          }

          if ((ucs2_pattern2_high - ucs2_pattern2_low) > 127)
          {
            /* cannot convert to 0x82 format - check if this is done in GSM7 already*/
            if (curr_gsm7_length == 0)
            {
              /* not possible in GSM7 also - mark its as not possible in 0x82 format */
              use_pattern2 = FALSE;
            }
            else
            {
              ucs82_len += curr_gsm7_length;
            }
          }
          else
          {
            if (curr_gsm7_length == 0)
            {
              ucs82_len++;
            }
            else
            {
              ucs82_len += curr_gsm7_length;
            }
          }
        }

        /* If we are still trying 0x82 format, it can convert this character. */
        if (use_pattern2)
        {
          ucs82_converts++;
        }
      }

      /* We can always do 0x80 format, assuming there is room. */
      if ((ucs80_len + 2) <= output_size)
      {
        ucs80_len += 2;
        ucs80_converts++;
      }
    }

    /* Check for the condition when we have no more room in the ouput string.
       The checks are done in the order of how efficient each is at storing
       results.  The end goal will be to determine which method would store
       the most characters.  Each check, in the order preseted, checks if we
       are still adding to this entry, followed by a check if there is room.*/
    if (all_gsm7)
    {
      if (gsm7_len >= output_size)
      {
        break;
      }
    }
    else if (use_pattern1)
    {
      if (ucs81_len >= output_size)
      {
        break;
      }
    }
    else if (use_pattern2)
    {
      if (ucs82_len >= output_size)
      {
        break;
      }
    }
    else if (ucs80_len >= output_size) 
    {
      break;
    }
  }

  /* Start by assuming the best coding turns all characters to GSM7 */
  best_converts = gsm7_converts;
  *out_base = 0;
  *out_mode = SIM_CODING_GSM7;
  *out_len = gsm7_len;
  *out_converts = best_converts;

  if (!gsm7_only)
  {
    /* If coding 81 encoded more characters we use that one. */
    if (ucs81_converts > best_converts)
    {
      /* All characters meet GSM7 or the bit pattern: 0hhh hhhh hXXX XXXX? */
      best_converts = ucs81_converts;
      *out_base = ucs2_pattern1;
      *out_mode = SIM_CODING_UCS2_HALF_PAGE_1;
      *out_len = ucs81_len;
      *out_converts = best_converts;
    }

    /* If coding 82 encoded more characters we use that one. */
    if (ucs82_converts > best_converts)
    {
      /* All characters meet GSM7 or are the remaining ones within 128 of each other? */
      best_converts = ucs82_converts;
      *out_base = ucs2_pattern2_low;
      *out_mode = SIM_CODING_UCS2_HALF_PAGE_2;
      *out_len = ucs82_len;
      *out_converts = best_converts;
    }

    /* If coding 80 encoded more characters we use that one. */
    if (ucs80_converts > best_converts)
    {
      /* If not, use 0x80 type.*/
      best_converts = ucs80_converts;
      *out_base = 0;
      *out_mode = SIM_CODING_PURE_UCS2;
      *out_len = ucs80_len;
      *out_converts = best_converts;
    }
  }
} /* pbmconvert_parse_input */


/*===========================================================================
FUNCTION find_gsm_ext

DESCRIPTION
  boolean find_gsm_ext(uint8 second_octet, uint16 *new_value)

  second_octet: [in]  Octet to search for.
  new_value:    [out] Unicode value corresponding to input.

  This function is used to search the gsm7ext_to_unicode table
  for the specified second octet of a sequence 0x1B xx.  If found
  it fills in new_value, if not, it returns FALSE.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE Indicates the value exists in the secondary table.
  FALSE indicates the value does not exist in that table.

SIDE EFFECTS
  None.
===========================================================================*/
static boolean find_gsm_ext(uint8 second_octet, uint16 *new_value)
{
   uint32 i;

   PBM_CHECK_PTR_RET(new_value, FALSE);

   /* Since the gsm7ext_to_unicode table is not in numerical order,
    * we must search in linearly. */
   for (i = 0; i < ARR_SIZE(gsm7ext_to_unicode); i++)
   {
      if (gsm7ext_to_unicode[i].octet == second_octet)
      {
         *new_value = gsm7ext_to_unicode[i].Unicode;
         return TRUE;
      }
   }

   return FALSE;
}

/*===========================================================================
FUNCTION convert_gsm7_to_unicode

DESCRIPTION
  void convert_gsm7_to_unicode(uint8 gsm7_char, uint16 *ucs2_char)

  gsm7_char: [in] GSM7 Character to search for.
  ucs2_char: [out] Unicode value corresponding to GSM character.

  This function gets the Unicode value from primary gsm7_to_unicode table.
  This function relies upon the fact that the first 128 entries in this
  table correspond to the first 128 GSM7 characters.

  If the gsm7_character has the high bit set, it is removed.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void convert_gsm7_to_unicode(uint8 gsm7_char, uint16 *ucs2_char)
{
   PBM_CHECK_PTR_RET(ucs2_char, VOID);

   gsm7_char &= 0x7F;

   *ucs2_char = gsm7_to_unicode[gsm7_char].Unicode;
}

/*===========================================================================
FUNCTION pbmconvert_convert_one_ucs2_to_gsm7

DESCRIPTION
  boolean pbmconvert_convert_one_ucs2_to_gsm7(uint16 ucs2_char,
                                              uint8 *gsm7_ptr, uint16 *gsm7_len)

  ucs2_char: [in]     The ucs2 character to search for.
  *gsm7_ptr: [out]    The corresponding GSM7 character.
  *gsm7_len: [in/out] On Input, this is the number of octets at gsm7_ptr
                      to which we can write.  On return, this item indicates
                      how many octets we did write.

  This function will search the unicode_to_gsm7 table for the
  provided ucs2 character.  If found, it will write the assoicated
  octet(s) to the gsm7_ptr.  In the case where the character is stored
  in the extension, this sequence "1B xx" is written to the gsm7_ptr;

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if we found the ucs2 character in GSM7 alphabet..
  FALSE if we did not find the ucs2 character in GSM7 alphabet..

SIDE EFFECTS
  None.
===========================================================================*/
static boolean pbmconvert_convert_one_ucs2_to_gsm7(uint16 ucs2_char,
                                                   uint8 *gsm7_ptr,
                                                   uint16 *gsm7_len)
{
   uint32 index;                                // Current index being searched.
   uint16 output_size;                          // Max size we can write.

   CONVERSION_TABLE *search_result;             // Bsearch result.
   CONVERSION_TABLE key;                        // Bsearch Key.

   /* First verify the parameters are good.  If either pointer is NULL,
    * we will to the search, but only return that something was found . */
   if (!gsm7_ptr || !gsm7_len)
      output_size = 0;
   else
      output_size = *gsm7_len;

   /* Initialize the key values. */
   key.octet = 0;
   key.Unicode = ucs2_char;
   search_result = (CONVERSION_TABLE *) bsearch(&key, unicode_to_gsm7,
                                                ARR_SIZE(unicode_to_gsm7),
                                                sizeof(CONVERSION_TABLE),
                                                pbmconvert_unicode_compare);
   if (search_result != NULL)
   {
      /* Save the octet in the provided buffer. */
      if ((output_size > 0) && (gsm7_ptr))
         *gsm7_ptr = search_result->octet;

      if (gsm7_len)
         *gsm7_len = 1;

      return TRUE;
   }

   /* If it is not yet found, it may be in the extension file. */
   /* Search the entire table, its small enough. */
   for (index = 0; index < ARR_SIZE(gsm7ext_to_unicode); index++)
   {
      if (gsm7ext_to_unicode[index].Unicode == ucs2_char)
      {
         if (output_size >= 2)
         {
            /* We found it and can save it. */
            if (gsm7_ptr)
            {
               *gsm7_ptr++ = 0x1B;
               *gsm7_ptr++ = gsm7ext_to_unicode[index].octet;
            }
            if (gsm7_len)
               *gsm7_len = 2;
            return TRUE;
         } else
         {
            /* We found it, but could not save it. */
            if (gsm7_len)
               *gsm7_len = 0;
         }

         /* But we found it! */
         return TRUE;
      }
   }

   /* If we reach here, it was not found. */
   if (gsm7_len)
      *gsm7_len = 0;
   return FALSE;
}

/*===========================================================================
FUNCTION pbmconvert_convert_ucs2_to_gsm7

DESCRIPTION
  boolean pbmconvert_convert_ucs2_to_gsm7( uint16 in_ucs2, uint8 **out_octets,
                                           uint16 *out_len, uint16 out_base)

  in_ucs2:    [in]      The UCS2 character to convert to SIM GSM7.
  out_octets: [out]     Octets as stored in SIM.  **out_octets,
                        gets the octets for the SIM, *out_octets
                        is the next pointer to use.
  out_len:    [in/out]  How many octets we have left.
  out_base:   [in]      Not used in this coding scheme.

  This function is called when converting a UCS2 string to GSM7 on the
  UIM.   It will convert the given UCS2 character and return TRUE if
  it thinks we are done converting the string.

DEPENDENCIES
  None.

RETURN VALUE
  True when we are done filling the field.
  False when we have more to do.

SIDE EFFECTS
  None.
===========================================================================*/
//lint -e{715} suppress unused parameters
static boolean pbmconvert_convert_ucs2_to_gsm7(uint16 in_ucs2,
                                               uint8 **out_octets,
                                               uint16 *out_len, uint16 out_base)
{
   uint16 octets_written = 0; // Number of octets written.

   /* Check input parameters. */
   PBM_CHECK_PTR_RET(out_octets, TRUE);
   PBM_CHECK_PTR_RET(*out_octets, TRUE);
   PBM_CHECK_PTR_RET(out_len, TRUE);

   if (*out_len > 0)      // If there is room to try.
   {
      /* Save the number of octets that it can write. */
      octets_written = *out_len;
      if (!pbmconvert_convert_one_ucs2_to_gsm7(in_ucs2, *out_octets,
                                               &octets_written))
      {
         /* Nothing was converted, end of usable string. */
         return TRUE;
      }

      /* Something was most likely written, update */
      /* values for the caller. */
      *out_len -= octets_written;
      *out_octets += octets_written;
   }

   /* If no more room, or there wasn't enough room, we are done. */
   if (*out_len == 0 || octets_written == 0)
      return TRUE;

   return FALSE;
}

/*===========================================================================
FUNCTION pbmconvert_convert_ucs2_to_ucs2_80

DESCRIPTION
  boolean pbmconvert_convert_ucs2_to_ucs2_80 (uint16 in_ucs2,
                                              uint8 **out_octets,
                                              uint16 *out_len, uint16 out_base)

  in_ucs2:     [in]      The unicode character to be saved.
  out_octets:  [out]     The resulting string.  Upon return **out_octets
                         contains the newly encoded octets, and *out_octets
                         contains the next place to write.
  out_len:     [in/out]  The number of octets we can write on the way in,
                         The number of octets remaining on the way out.
  out_base:    [N/A]     Not used for this coding scheme.

  This encodes the characters in the manner required when the first octet
  of the alpha field is 0x80.   Each UCS2 character is directly output
  to the resulting string with no conversion.

DEPENDENCIES
  None.

RETURN VALUE
  True is returned if we are done converting the string.
  False is returned if there is more to do.

SIDE EFFECTS
  None.
===========================================================================*/
static boolean pbmconvert_convert_ucs2_to_ucs2_80 (uint16 in_ucs2,
                                                   uint8 **out_octets,
                                                   uint16 *out_len,
                                                   uint16 out_base)
{
  (void)out_base;

   /* Check input parameters. */
   PBM_CHECK_PTR3_RET(out_octets, *out_octets, out_len, TRUE);

   /* Simple ucs2 -> ucs2 encoding.  Just need two octets left. */
   if (*out_len >= 2)
   {
      /* Write the MSB */
      **out_octets = (in_ucs2 & 0xFF00) >> 8;
      *out_octets += 1;
      /* Write the LSB */
      **out_octets = (in_ucs2 & 0x00FF) >> 0;
      *out_octets += 1;

      /* We wrote two octets.*/
      *out_len -= 2;
   }

   /* When we have than 2 remaining, we are done. */
   if (*out_len < 2)
      return TRUE;

   return FALSE;
}

/*===========================================================================
FUNCTION pbmconvert_convert_ucs2_to_ucs2_81

DESCRIPTION
  boolean pbmconvert_convert_ucs2_to_ucs2_81(uint16 in_ucs2, uint8 **out_octets,
                                             uint16 *out_len, uint16 out_base)

  in_ucs2:     [in]      The unicode character to be saved.
  out_octets:  [out]     The resulting string.  Upon return **out_octets
                         contains the newly encoded octets, and *out_octets
                         contains the next place to write.
  out_len:     [in/out]  The number of octets we can write on the way in,
                         The number of octets remaining on the way out.
  out_base:    [in]      Based to which characters with high bit set are
                         ORred with.

  This encodes the characters in the manner required when the first octet
  of the alpha field is 0x81.   Each UCS2 character is categorized as either
  GSM7 or not.  If the character can be GSM7, it is stored as such.  If not
  then the out_base bits are removed from the character, and the resulting
  character must be 0-7F.  The high bit is set and that character is then
  written.

DEPENDENCIES
  None.

RETURN VALUE
  True is returned if we are done converting the string.
  False is returned if there is more to do.

SIDE EFFECTS
  None.
===========================================================================*/
static boolean pbmconvert_convert_ucs2_to_ucs2_81 (uint16 in_ucs2,
                                                   uint8 **out_octets,
                                                   uint16 *out_len,
                                                   uint16 out_base)
{
   uint16 octets_written = 0;    /* Used when asking to convert to GSM7 */

   /* Check input parameters. */
   PBM_CHECK_PTR_RET(out_octets, TRUE);
   PBM_CHECK_PTR_RET(*out_octets, TRUE);
   PBM_CHECK_PTR_RET(out_len, TRUE);

   if (*out_len > 0)
   {
      octets_written = *out_len;  // Save how much room we have. */
         /* Couldn't be written as GSM 7, check that it can encode accoringly */
         if ((out_base | (in_ucs2 & 0x007F)) == in_ucs2)
         {
            /* It can be written as UCS2 in this coding scheme. Write the
             * value.  */
            **out_octets = 0x80 | (in_ucs2 & 0x007F);
            octets_written = 1;
         } else
         {
            /* It could not be encoded correctly, return that we are done. */
            if (!pbmconvert_convert_one_ucs2_to_gsm7(in_ucs2, *out_octets,&octets_written))
            {
            return TRUE;
         }
      }

      /* Store what we did and move the pointer appriately. */
      *out_len -= octets_written;
      *out_octets += octets_written;
   }

   /* If we have no room, or couldn't do what we need, we are done. */
   if ((*out_len == 0) || (octets_written == 0))
      return TRUE;

   return FALSE;
}

/*===========================================================================
FUNCTION pbmconvert_convert_ucs2_to_ucs2_82

DESCRIPTION
  boolean pbmconvert_convert_ucs2_to_ucs2_82 (uint16 in_ucs2,
                                              uint8 **out_octets,
                                              uint16 *out_len, uint16 out_base)

  in_ucs2:     [in]      The unicode character to be saved.
  out_octets:  [out]     The resulting string.  Upon return **out_octets
                         contains the newly encoded octets, and *out_octets
                         contains the next place to write.
  out_len:     [in/out]  The number of octets we can write on the way in,
                         The number of octets remaining on the way out.
  out_base:    [in]      Based to which characters with high bit set are
                         ORred with.

  This encodes the characters in the manner required when the first octet
  of the alpha field is 0x82.   Each UCS2 character is categorized as either
  GSM7 or not.  If the character can be GSM7, it is stored as such.  If not
  then the out_base is subtracted from the character, and the resulting
  character value must be 0-7F.  The high bit is set and that octet is then
  written.

DEPENDENCIES
  None.

RETURN VALUE
  True is returned if we are done converting the string.
  False is returned if there is more to do.

SIDE EFFECTS
  None.
===========================================================================*/
static boolean pbmconvert_convert_ucs2_to_ucs2_82 (uint16 in_ucs2,
                                                   uint8 **out_octets,
                                                   uint16 *out_len,
                                                   uint16 out_base)
{
   uint16 octets_written = 0; // Number of octets written.

   /* Check input parameters. */
   PBM_CHECK_PTR_RET(out_octets, TRUE);
   PBM_CHECK_PTR_RET(*out_octets, TRUE);
   PBM_CHECK_PTR_RET(out_len, TRUE);

   if (*out_len > 0)
   {
      octets_written = *out_len;  // Save how much room we have. */
         /* Couldn't be written as GSM 7, check that it can encode accoringly */
         if (out_base + ((in_ucs2 - out_base) & 0x7F) == in_ucs2)
         {
            /* It can be written as UCS2 in this coding scheme. Write the
             * value.  */
            **out_octets = 0x80 | ((in_ucs2 - out_base) & 0x007F);
            octets_written = 1;
         } else
         {
            /* It could not be encoded correctly, return that we are done. */
             if (!pbmconvert_convert_one_ucs2_to_gsm7(in_ucs2, *out_octets,&octets_written))
             {
                return TRUE;
              }
         }
      /* Store what we did and move the pointer appriately. */
      *out_len -= octets_written;
      *out_octets += octets_written;
   }

   /* If we have no room, or couldn't do what we need, we are done. */
   if ((*out_len == 0) || (octets_written == 0))
      return TRUE;

   return FALSE;
}


/*===========================================================================
FUNCTION pbmconvert_sim_ucs2_to_ucs2

DESCRIPTION
  boolean pbmconvert_sim_ucs2_to_ucs2(uint8 **in_string, uint16 *in_len,
                                      uint16 **out_octets, uint16 ucs2_base)

  in_string:  [in/out]  On input, **in_string is next octet to convert,
                        On output, *in_string is moved to point to the next one.
  in_len:     [in/out]  On input, the number of characters we can convert.
                        On output, the number of characters left to convert.
  out_octets: [in/out]  On input, where to put the ucs2 character.
                        On ouput, incremented by two.
  ucs2_base:  [in]      Not used in this conversion.

  This routine will read the next two octets from the UIM string
  and store those as UCS2.  In this coding scheme, no further processing
  is necessary to get the UCS2 character.  This routine looks ahead
  two more characters and if they are 0xFFFF, then we are done.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if this routine has processed the last of the characters in in_string.
  FALSE if there is more work to do.

SIDE EFFECTS
  None.
===========================================================================*/
//lint -e{715} suppress unused parameters
static boolean pbmconvert_sim_ucs2_to_ucs2(uint8 **in_string,
                                           uint16 *in_len,
                                           uint16 **out_octets,
                                           uint16 ucs2_base)
{
   uint16 ucs2_char;     // Character represented in UIM string.

   /* Check input parameters. */
   PBM_CHECK_PTR5_RET(in_string, *in_string, out_octets, *out_octets, in_len, TRUE);

   if (*in_len >= 2)     // If there are two octets, they are UCS2.
   {
      /* Save the character. */
      ucs2_char = (**in_string << 8) | *((*in_string) + 1);

      /* Move the input along to indicate we have processed these. */
      *in_string += 2;
      *in_len -= 2;

      /* Save the ucs2 character to the string. */
      **out_octets = ucs2_char;

      /* Increment the output buffer past where we saved the data. */
      *out_octets += 1;
   }

   /* If there are 0 or 1 characters left, there are no more UCS2 chars */
   /* Also, if we would encounter 0xFFFF next, we are done as well. */
   if ((*in_len > 1) && ((**in_string != 0xFF) || (*((*in_string) + 1) != 0xFF)))
      return FALSE;

   return TRUE;
}


/*===========================================================================
FUNCTION pbmconvert_sim_gsm7_to_ucs2

DESCRIPTION
  boolean pbmconvert_sim_gsm7_to_ucs2(uint8 **in_string, uint16 *in_len,
                                      uint16 **out_octets, uint16 ucs2_base)

  in_string:  [in/out]  On input, **in_string is next octet to convert,
                        On output, *in_string is moved to point to the next one.
  in_len:     [in/out]  On input, the number of characters we can convert.
                        On output, the number of characters left to convert.
  out_octets: [in/out]  On input, where to put the ucs2 character.
                        On ouput, incremented by two.
  ucs2_base:  [in]      Not used in this conversion.

  This function takes the input string and converts the next GSM7 encoded
  character to UCS2.  The input may be one, two, or three octets in length
  depending upon if the input contains 0x1b.  In the case of 0x1B, it is
  an extension record and checks the next table.  0x1b1b looks at the non
  existent 3rd table.  If we cannot convert the character, it is encoded
  as <space><char&0x7f> or <space><space><space><char&0x7f>

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if this routine has processed the last of the characters in in_string.
  FALSE if there is more work to do.

SIDE EFFECTS
  None.
===========================================================================*/
//lint -e{715} suppress unused parameters
static boolean pbmconvert_sim_gsm7_to_ucs2(uint8 **in_string,
                                           uint16 *in_len,
                                           uint16 **out_octets,
                                           uint16 ucs2_base)
{
   uint8 curr_value;             /* Current octet being looked at. */

   /* Check input parameters. */
   PBM_CHECK_PTR5_RET(in_string, *in_string, out_octets, *out_octets, in_len, TRUE);

   if ((*in_len >= 1) && ((curr_value = **in_string) != 0xFF))
   {
      *in_string += 1;        // We looked at one character.
      if (curr_value & 0x80)  // If the high bit is set, somethings wrong.
      {
         UIM_MSG_HIGH_1("Unexpected character with high bit set 0x%x",curr_value);
         curr_value &= 0x7F;   // Ignore it for this type of coding.
      }

      if (curr_value == 0x001B)  /* Extension. */
      {
         if (*in_len >= 2) /* If there is a next octet, look for that. */
         {
            uint8 second_octet = **in_string;
            *in_string += 1;

            /* Is this second octet in the extension field? */
            if (find_gsm_ext(second_octet, *out_octets))
            {
               /* Found the second octet, out_octets as been written */
               *out_octets += 1;
               *in_len -= 2;   /* It took two characters to get one UCS2 */
            } else
            {
               /* Unknown escape sequence. */
               if (second_octet == 0x1b)
               {
                  /* If we support a third table, we would check if the second
                   * character is an escape as well, and that would go here. */
                  convert_gsm7_to_unicode(0x1b, (uint16 *) *out_octets);
                  *out_octets+=1;
                  convert_gsm7_to_unicode(0x1b, (uint16 *) *out_octets);
                  *out_octets+=1;
                  *in_len-=2;
                  /* We do not have a 3rd table, so Escape Escape turns into
                   * Space Space.  The 3rd octet will be written as Unicode
                   * stripping off a high bit if set.  If its not there, we
                   * will end with <space><space>
                   */
                  if (*in_len >= 1)
                  {
                     /* Convert the last octet as a character. */
                     convert_gsm7_to_unicode((**in_string & 0x7f),
                                             (uint16 *) *out_octets);
					 *in_string += 1;
                     *out_octets+=1;
                     *in_len-=1;
                  }
               } else
               {
                  /* This is the case where an extra item may have been
                   * added to the ext table.  Since we don't understand it,
                   * it is written as <space><char>.  The idea is the <char>
                   * should be somewhat close to what was intended.  I.e.
                   * the Euro symbol would look like an E if we didn't know
                   * about it.
                   */
                  convert_gsm7_to_unicode(0x1b, (uint16 *) *out_octets);
                  *out_octets+=1;

                  convert_gsm7_to_unicode((second_octet & 0x7f),
                                          (uint16 *) *out_octets);
                  *out_octets+=1;

                  *in_len-=2;  /* Two octets were looked at. */
               }
            }
         } else
         {
            /* Code ended with 0x1b, just print the space. */
            convert_gsm7_to_unicode(0x1b, (uint16 *) *out_octets);
            *out_octets+=1;
            *in_len-=1;
         }
      } else
      {
         /* A good old fasioned GSM character. */
         convert_gsm7_to_unicode(curr_value, (uint16 *) *out_octets);
         *out_octets+=1;
         *in_len-=1;
      }
   }

   if ((*in_len >= 1) && ((curr_value = **in_string) != 0xFF))
      return FALSE;
   return TRUE;
}

/*===========================================================================
FUNCTION pbmconvert_sim_ucs2_halfpage_to_ucs2

DESCRIPTION
  boolean pbmconvert_sim_ucs2_halfpage_to_ucs2(uint8 **in_string,
                                               uint16 *in_len,
                                               uint16 **out_octets,
                                               uint16 ucs2_base)

  in_string:  [in/out]  On input, **in_string is next octet to convert,
                        On output, *in_string is moved to point to the next one.
  in_len:     [in/out]  On input, the number of characters we can convert.
                        On output, the number of characters left to convert.
  out_octets: [in/out]  On input, where to put the ucs2 character.
                        On ouput, incremented to the next output location.
  ucs2_base:  [in]      Base to which the encoded character is orred.

  This routine will grab the next octet from the input and convert it to
  at least one ucs2 character.  If the character has the high bit set, it
  is encoded by adding the low 7 bits of the octet to the base.  If the
  character does not have the high bit set, it is encoded as GSM7.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if this routine has processed the last of the characters in in_string.
  FALSE if there is more work to do.

SIDE EFFECTS
  None.
===========================================================================*/
static boolean pbmconvert_sim_ucs2_halfpage_to_ucs2(uint8 **in_string,
                                                    uint16 *in_len,
                                                    uint16 **out_octets,
                                                    uint16 ucs2_base)
{
   uint16 curr_value = 0;

  /* Check input parameters. */
  PBM_CHECK_PTR5_RET(in_string, *in_string, out_octets, *out_octets, in_len, TRUE);

   if (*in_len >= 1)
   {
      curr_value = **in_string;
      if (curr_value & 0x80)  // If the high bit is set, add this to ucs2_base.
      {
         *in_string += 1;
         **out_octets = (curr_value & 0x7F) + ucs2_base;
         *out_octets += 1;
         *in_len -= 1;
      } else
      {
         /* The octet is a GSM7 encoded one, use the function for that one.
          * The gsm7 function can not tell when we are done, so ignore
          * its return code. */
         (void) pbmconvert_sim_gsm7_to_ucs2(in_string, in_len, out_octets,
                                            ucs2_base);
      }
   }

   if (*in_len > 0)
      return FALSE;
   return TRUE;
}

/*===========================================================================
FUNCTION pbmconvert_sim_to_string

DESCRIPTION
  uint32 pbmconvert_sim_to_string(uint8 *in_str, uint16 in_len,
                                  uint16 *out_octets, uint16 out_len)

  in_str:     [in]   SIM encoded octets to be converted to a UCS2 string.
  in_len:     [in]   Length of the above string.  Actual usable info may
                     be less.
  out_octets: [out]  Place where results are put.
  out_len:    [in]   Number of UCS2 characters at out_octets.

  This function will read the octets that were in the SIM and convert it
  to a UCS2 string.

DEPENDENCIES
  None.

RETURN VALUE
  The number of UCS2 characters in the resulting string.

SIDE EFFECTS
  None.
===========================================================================*/
uint32 pbmconvert_sim_to_string(uint8 *in_str, uint16 in_len,
                                uint16 *out_octets, uint16 out_len)
{
   SIM_CONVERSION octet_fn;      /* Function to convert a octet to UCS2. */
   uint16 ucs2_base = 0x0000;    /* Used with UCS2_HALF_PAGE scemes */
   uint16 octets_to_convert = 0; /* Used when length specified internally */
   boolean done;                 /* Are we done. */
   uint32 retval = 0;            /* Number of octets converted. */

   /* Check input parameters. */
   PBM_CHECK_PTR2_RET(in_str, out_octets, retval);

   /* first figure out how this entry is stored. */
   switch (*in_str)
   {
      case 0x80:
         in_str+=UCS80_LEN_OH;
         octets_to_convert = in_len-UCS80_LEN_OH;
         octet_fn = pbmconvert_sim_ucs2_to_ucs2;
         break;
      case 0x81:
         octets_to_convert = *(in_str+1);
         ucs2_base = *(in_str+2) << 7;
         in_str+=UCS81_LEN_OH;
         octet_fn = pbmconvert_sim_ucs2_halfpage_to_ucs2;
         break;
      case 0x82:
         octets_to_convert = *(in_str+1);
         ucs2_base = (*(in_str+2) << 8) | *(in_str+3);
         octet_fn = pbmconvert_sim_ucs2_halfpage_to_ucs2;
         in_str+=UCS82_LEN_OH;
         break;
      default:
         octets_to_convert = in_len;
         octet_fn = pbmconvert_sim_gsm7_to_ucs2;
         break;
   }

   /* Setup a function that converts an in_type to an out_type */
   do
   {
      done = (*octet_fn)(&in_str, &octets_to_convert, &out_octets, ucs2_base);
      retval += 1;
      if (retval >= out_len)
         break;
   } while (!done && (octets_to_convert > 0));

   return retval;
}


/*===========================================================================
FUNCTION pbmconvert_unicode_compare

DESCRIPTION
  int pbmconvert_unicode_compare(const void *first_arg, const void *second_arg)

  This function is called at initialization.  It builds the secondary
  tables that are sorted in Unicode order to facilitate searching.  The
  searching is done as a Binary Search, thus the table must be sorted.
  This is the comparitor function for sorting and searching.

DEPENDENCIES
  None.

RETURN VALUE
  > 0 if the first is greater than the first.
  < 0 if the second is greater to the first.
  = 0 if the first and second argument have the same Unicode value.

SIDE EFFECTS
  None.
===========================================================================*/
static int pbmconvert_unicode_compare(const void *first_arg,
                                      const void *second_arg)
{
   CONVERSION_TABLE *first = (CONVERSION_TABLE *) first_arg;
   CONVERSION_TABLE *second = (CONVERSION_TABLE *) second_arg;

   PBM_CHECK_PTR2_RET(first, second, 0);

   return(first->Unicode - second->Unicode);
}

/*===========================================================================
FUNCTION pbmconvert_init_conversion

DESCRIPTION
  void pbmconvert_init_conversion()

  This function is called at initialization.  It builds the secondary
  tables that are sorted in Unicode order to facilitate searching.  The
  searching is done as a Binary Search, thus the table must be sorted.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void pbmconvert_init_conversion(void)
{
   // copy pbm8_to_unicode into unicode_to_gsm7;
   memscpy(unicode_to_pbm8, sizeof(unicode_to_pbm8), pbm8_to_unicode, sizeof(unicode_to_pbm8));

   // sort unicode_to_pbm8 by the unicode character;
   qsort(unicode_to_pbm8, ARR_SIZE(unicode_to_pbm8), sizeof(CONVERSION_TABLE),
         pbmconvert_unicode_compare);

   // copy gsm7_to_unicode into unicode_to_gsm7;
   memscpy(unicode_to_gsm7, sizeof(unicode_to_gsm7), gsm7_to_unicode, sizeof(unicode_to_gsm7));

   // sort unicode_to_gsm7 by the unicode character;
   qsort(unicode_to_gsm7, ARR_SIZE(unicode_to_gsm7), sizeof(CONVERSION_TABLE),
         pbmconvert_unicode_compare);

}

/*===========================================================================
FUNCTION pbmconvert_pbm8_source

DESCRIPTION
  uint16 pbmconvert_pbm8_source(const void **in_ptr)

  in_ptr: [in/out] Source pointer that is moved after we get the octet.

  The utilities in this file use Unicode as the basis for the conversion.
  When presented with a pbm8 string, this input function is used to get
  each octet, convert it quickly to unicode, and return that.

DEPENDENCIES
  None.

RETURN VALUE
  Unicode value associated with the pbm8 value..

SIDE EFFECTS
  None.
===========================================================================*/
static uint16 pbmconvert_pbm8_source(const void **in_ptr)
{
   uint8  *input;
   uint8  pbm8_char;
   uint16 ret_val = 0;

   /* Check input parameters. */
   PBM_CHECK_PTR2_RET(in_ptr, *in_ptr, ret_val);

   input = (uint8 *) *in_ptr;

   /* Take the in_ptr as a character. */
   pbm8_char = *input;

   /* Move in_ptr one octet. */
   input++;

   /* Assign the in_ptr to be the new pointer value. */
   *in_ptr = (void *) input;

   /* Convert what we got to Unicode. */
   ret_val = pbm8_to_unicode[pbm8_char].Unicode;

   /* Return that. */
   return ret_val;
}

/*===========================================================================
FUNCTION pbmconvert_return_input_string_first_ucs2_char

DESCRIPTION
  This function takes the PBM8 or UCS2 string, gets the UCS2 character and
  then increment the pointer by two. It returns the first converted UCS2 
  character.

  in_ptr: [in/out] Source pointer that is moved after we get the octet.


DEPENDENCIES
  None.

RETURN VALUE
  Unicode value at in_ptr;

SIDE EFFECTS
  None.
===========================================================================*/
uint16 pbmconvert_return_input_string_first_ucs2_char(const void **in_ptr)
{
   uint16 ret_val = 0;
   uint16 *input;

   /* Check input parameters. */
   PBM_CHECK_PTR2_RET(in_ptr, *in_ptr, ret_val);

   /* Set input to the octets of *in_ptr. */
   input = (uint16 *) *in_ptr;

   /* Get the return pointer, increment buffer by a uint16. */
   ret_val = *input++;

   /* Set *in_ptr to the next input octets. */
   *in_ptr = (void *) input;

   return ret_val;
}

/*===========================================================================
FUNCTION pbmconvert_convert_one_ucs2_to_pbm8

DESCRIPTION
  boolean pbmconvert_convert_one_ucs2_to_pbm8(uint16 ucs2_char, uint8 *pbm8_ptr)

  ucs2_char: [in]  UCS2 character to convert.
  *pbm8_ptr: [out] Resulting PBM8 character is stored in here.

  This function does a binary search on the unicode_to_pbm8 array.  If
  it find the matching unicode character, the pbm8 character is written
  in the provided output buffer.

DEPENDENCIES
  None.

RETURN VALUE
  True if the UCS2 character was found.
  False if the UCS2 character was not found.

SIDE EFFECTS
  None.
===========================================================================*/
static boolean pbmconvert_convert_one_ucs2_to_pbm8(uint16 ucs2_char,
                                                   uint8 *pbm8_ptr)
{
   CONVERSION_TABLE *search_result;
   CONVERSION_TABLE key;

   key.octet = 0;
   key.Unicode = ucs2_char;
   search_result = (CONVERSION_TABLE *) bsearch(&key, unicode_to_pbm8,
                                                ARR_SIZE(unicode_to_pbm8),
                                                sizeof(CONVERSION_TABLE),
                                                pbmconvert_unicode_compare);
   if (search_result != NULL)
   {
      /* Save the octet in the provided buffer. */
      if (pbm8_ptr)
         *pbm8_ptr = search_result->octet;
      return TRUE;
   }

   /* If we found a match, we return from the middle. */
   return FALSE;
}

/*===========================================================================
FUNCTION pbmconvert_ucs2_to_pbm8

DESCRIPTION
  boolean pbmconvert_ucs2_to_pbm8(uint16 *ucs2_str, uint16 ucs_str_len,
                                  uint8 *pbm8_str, uint16 pbm8_str_len)
  *ucs2_str:    [in]  Array of UCS2 characters to convert to PBM8.
  ucs_str_len:  [in]  Length of the array of UCS2 characters.
  *pbm8_str:    [out] Array where translation is saved.
  pbm8_str_len: [in]  Length of results string.

  This function will convert a UCS2 string to PBM8.  It will do its
  best to convert the entire string, but will fill unrecognized characters
  with the PBM8_UNDEFINED_CHAR define octet (Non-blocking space).

  The function will leave the resulting string as null terminated,
  truncating the source if necessary.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE indicates every character in ucs2_string exists in the resulting
       PBM8 string.
  FALSE if some character could not be translated or we ran out of room.

SIDE EFFECTS
  None.
===========================================================================*/
boolean pbmconvert_ucs2_to_pbm8(uint16 *ucs2_str, uint16 ucs_str_len,
                                uint8 *pbm8_str, uint16 pbm8_str_len)
{
   int ucs2_pos = 0;       /* Position in UCS2 string. */
   uint16 *ucs2_ptr;       /* Pointer to ucs2_string. */
   uint8 *pbm8_ptr;        /* Pointer to the pbm8 string. */
   boolean ret_val = TRUE; /* Can every character be converted. */
   uint8 pbm8_char;        /* Character that it maps to. */

   /* Parameter checking. */
   PBM_CHECK_PTR2_RET(ucs2_str, pbm8_str, FALSE);

   /* Initialization. */
   ucs2_ptr = ucs2_str;   /* Pointer to where we will get the string. */
   pbm8_ptr = pbm8_str;   /* Pointer to where we will put the results. */

   /* For every character, see if we can map it to pbm8. */
   for (ucs2_pos = 0; ucs2_pos < ucs_str_len; ucs2_pos++)
   {
      /* Find the UCS2 value in PBM8, if its not found, that changes our
       * return code. Either way, we expect pbm8_char to be set to a valid
       * PBM8 value. */
      if (!pbmconvert_convert_one_ucs2_to_pbm8(*ucs2_ptr++, &pbm8_char))
      {
         ret_val = FALSE;
         pbm8_char = PBM8_UNDEFINED_CHAR;
      }

      /* If we have room for this and NULL, add the octet */
      if (pbm8_str_len > 1)
      {
         pbm8_str_len--;
         *pbm8_ptr++ = pbm8_char;
      } else
      {
         ret_val = FALSE;
         break;
      }
   }

   if (pbm8_str_len > 0)
      *pbm8_ptr = '\0';

   return ret_val;
}

/*===========================================================================
FUNCTION pbmconvert_pbm8_to_ucs2

DESCRIPTION
  boolean pbmconvert_pbm8_to_ucs2( uint8 *pbm8_str,  uint16 pbm8_str_len,
                                   uint16 *ucs2_str, uint16 ucs2_str_len)

  pbm8_str:     [in]  PBM8 string to convert.
  pbm8_str_len: [in]  Length of said string.
  ucs2_str:     [out] Buffer filled with the translation.
  ucs2_str_len: [in]  Size of resulting buffers.  Note, the actual size
                      of the memory must be 2 times this value since each
                      item is 16 bits large.

  This function will convert a PBM8 string to UCS2.  The function will leave
  the resulting string as null terminated, truncating the source if necessary.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE indicates every character in pbm8_str exists in the resulting
       UCS2 string.
  FALSE if we ran out of room or got a bad param.

SIDE EFFECTS
  None.
===========================================================================*/
boolean pbmconvert_pbm8_to_ucs2(const uint8 *pbm8_str,  uint16 pbm8_str_len,
                                uint16 *ucs2_str, uint16 ucs2_str_len)
{
   int str_pos = 0;       /* Position in pbm8 and ucs2 strings. */
   boolean ret_val = TRUE; /* Can every character be converted. */
   uint16 ucs2_char;       /* Character that it maps to. */

   /* Parameter checking. */
   PBM_CHECK_PTR2_RET(pbm8_str, ucs2_str, FALSE);

   /* If there is no room, we ran out of room already. */
   if (ucs2_str_len == 0)
      return FALSE;

   /* For every character, see if we can map it to pbm8. */
   for (str_pos = 0; str_pos < pbm8_str_len; str_pos++)
   {
      /* The pbm8_to_unicode is exactly 256 bytes big.  All pbm8 values
       * are represented in Unicode here. */
      ucs2_char = pbm8_to_unicode[pbm8_str[str_pos]].Unicode;

      /* If we have room, add the octets */
      if (str_pos < ucs2_str_len - 1)  //keep room for NULL at end
      {
         ucs2_str[str_pos] = ucs2_char;
      } else
      {
         ret_val = FALSE;
         break;
      }
   }

   /* If they gave us a string with room, NULL terminate it. */
   if (str_pos < ucs2_str_len)
      ucs2_str[str_pos] = '\0';
   else  //stick a NULL at the end of the buffer we think we have
      ucs2_str[ucs2_str_len-1] = '\0';

   return ret_val;
}

/*===========================================================================
FUNCTION PBM_STRNCMP

DESCRIPTION
String compare function.  This function returns a number greater
than 0 if s1 is greater than s2, a number less than 0 if s1 is less than
s2, and 0 if s1 is the same as s2.  The comparison continues until both
strings reach the null character or n characters are compared. If
case_sensitive is true, the case of the characters must be identical.
If it is false, then a case-insensitive comparison is performed.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
int pbm_strncmp(const void *s1, const void *s2, size_t n,
                boolean case_sensitive,
                boolean s1_is_wstr, boolean s2_is_wstr)
{
   pfsource_fn src_fn1, src_fn2;
   int ret_val = 0;
   uint16 c1, c2; /* Characters used in comparison */
   /* While there are still characters to test AND
    * *s1 and *s2 are not both NULL characters AND
    * *s1 and *s2 are the same character (when in upper case)
    * check the next character.
    * Otherwise, return the result from the last comparison which has the
    * difference or, if there is no difference, it is set to the value 0.
    */
   if (s1_is_wstr)
   {
      src_fn1 = pbmconvert_return_input_string_first_ucs2_char;
   } else
   {
      src_fn1 = pbmconvert_pbm8_source;
   }
   if (s2_is_wstr)
   {
      src_fn2 = pbmconvert_return_input_string_first_ucs2_char;
   } else
   {
      src_fn2 = pbmconvert_pbm8_source;
   }

   while (n-- && (ret_val == 0))
   {
      c1 = ((*src_fn1)(&s1));
      c2 = ((*src_fn2)(&s2));
      if ((c1 == 0) && (c2 == 0))
         break;
      if (!case_sensitive)
      {
         c1 = pbm_wstr_tolower(c1);
         c2 = pbm_wstr_tolower(c2);
      }

      if ((c1 == c2) || (c1 == pbm8_to_unicode[PBM_WILD_CHAR].Unicode) ||
          (c2 == pbm8_to_unicode[PBM_WILD_CHAR].Unicode))
         ret_val = 0;
      else
         ret_val = (int) c1 - (int) c2;
   }

   return ret_val;
}

/*===========================================================================
FUNCTION PBM_STRSTR

DESCRIPTION

Locates the first occurence in the string pointed to by s1
of the sequence of characters in the string pointed to  by
s2 (excluding the terminating null character). The comparison
is case insensitive if case_sensitive is set to FALSE.

RETURNS

Returns a pointer to the located string segment, or a null
pointer if the string s2 is not found. If s2 points  to  a
string with zero length, then s1 is returned.

SIDE EFFECTS
===========================================================================*/
void *pbm_strstr(const void *s1, const void *s2, boolean case_sensitive,
                 boolean s1_is_wstr, boolean s2_is_wstr)
{
   const void *a;        /* a traverses s1 */
   const void *b;        /* b traverses s2 */
   uint16 c1, c2;
   pfsource_fn src_fn1, src_fn2;

   if (!s1 || !s2)
      return NULL;

   if (s1_is_wstr)
   {
      src_fn1 = pbmconvert_return_input_string_first_ucs2_char;
   } else
   {
      src_fn1 = pbmconvert_pbm8_source;
   }
   if (s2_is_wstr)
   {
      src_fn2 = pbmconvert_return_input_string_first_ucs2_char;
   } else
   {
      src_fn2 = pbmconvert_pbm8_source;
   }

   /* Use Knuth-Morris-Pratt algo ? */
   /* Inner loop goes on till there is no match or s2 is found and in that case
    * it returns. Outer loop increments the starting point on s1 for each failed
    * iteration.
    */
   a = s1;
   while ((*src_fn1)(&a) != 0)
   {
      a = s1;
      b = s2;
      do
      {
         /* Get characters for comparison. */
         c1 = (*src_fn1)(&a);
         c2 = (*src_fn2)(&b);

         if (c2 == 0x0000)
            return(void *) s1;
         if (c1 == 0x0000)
            return NULL;

         if (!case_sensitive)
         {
            /* Case insensitive searches, make both lower case. */
            c1 = pbm_wstr_tolower(c1);
            c2 = pbm_wstr_tolower(c2);
         }
      } while ((c1 == c2) || (c1 == pbm8_to_unicode[PBM_WILD_CHAR].Unicode) ||
               (c2 == pbm8_to_unicode[PBM_WILD_CHAR].Unicode));

      (void) (*src_fn1)(&s1);
   }

   return NULL;
}

/*===========================================================================
FUNCTION wstr_tolower

DESCRIPTION
  uint16 wstr_tolower(uint16 c)

  c: [in] UCS2 character to convert to lower case.  Currently, the
          only characters that are converted are 'A' - 'Z'.

  This function allows for a case insensitive match of strings.
  The current implementation only understands the LATIN character
  set, characters 'A' - 'Z', however, any character set may be added
  to this function.

RETURN VALUE
  Either the input character, or the lower case version of the same.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
uint16 pbm_wstr_tolower(uint16 c)
{
   if (c >= pbm8_to_unicode['A'].Unicode && c <= pbm8_to_unicode['Z'].Unicode)
   {
      return((c - pbm8_to_unicode['A'].Unicode) + pbm8_to_unicode['a'].Unicode);
   }

   return c;
}

