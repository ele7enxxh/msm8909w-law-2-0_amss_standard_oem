#ifndef PBM_CONVERSION_H
#define PBM_CONVERSION_H
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

  Copyright (c) 2005-2006,2016 byQUALCOMM Technologies, Inc(QTI).
  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/pbm/src/pbm_conversion.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/05/16   dd      Adding support for special chinese character encoding 
02/23/06   cvs     Remove incorrect header include, add string functions,
                   reduce interface
02/09/06   cvs     When can't encode all characters on SIM, return errors
11/15/05   cvs     Lint fixes
08/12/05   AT      Initial Revision
===========================================================================*/

#define PBM8_UNDEFINED_CHAR 0xA0    /* Non Blocking SPACE used when unknown
                                       UCS2 characters are converted to PBM8 */

#define GSM7_LEN_OH         0   /* Overhead for GSM7 Encoding in the SIM. */
#define UCS80_LEN_OH        1   /* Overhead if 0x80 format used. */
#define UCS81_LEN_OH        3   /* Overhead if 0x81 format used. */
#define UCS82_LEN_OH        4   /* Overhead if 0x82 format used. */

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
                              uint16 *out_converts, boolean gsm7_only);


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
                                uint16 *out_octets, uint16 out_len);


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
void pbmconvert_init_conversion(void);

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
                                uint8 *pbm8_str, uint16 pbm8_str_len);

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
                                uint16 *ucs2_str, uint16 ucs2_str_len);

/*===========================================================================
FUNCTION PBM_STRNCMP

DESCRIPTION
PBM String comparison function.  This function returns a number greater
than 0 if s1 is greater than s2, a number less than 0 if s1 is less than
s2, and 0 if s1 is the same as s2.  The comparison continues until both
strings reach the null character or n characters are compared.  Case sensitive
indicates whether or not to do a case sensitive comparison.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
int pbm_strncmp(const void *s1, const void *s2, size_t n,
                boolean case_sensitive,
                boolean s1_is_wstr, boolean s2_is_wstr);

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
                 boolean s1_is_wstr, boolean s2_is_wstr);

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
uint16 pbm_wstr_tolower(uint16 c);

#endif /* PBM_CONVERSION_H */
