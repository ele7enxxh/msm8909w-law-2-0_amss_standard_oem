/*===========================================================================

                        QM_UTIL.C

DESCRIPTION

  QMI_MMODE source file for utility functions module.

Copyright (c) 2013 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header:
  $DateTime:
  $Author:


when        who    what, where, why
--------    ---    ----------------------------------------------------------
01/25/13    sm     Initial version
===========================================================================*/

#include "qm_util.h"
#include "msg.h"
#include "mcfg_fs.h"
#include "dog_hb_rex.h"
#include "qmi_mmode_taski.h"

#define QM_UCS2_80 0x80
#define QM_UCS2_81 0x81
#define QM_UCS2_82 0x82

#define QM_MAX_F3_BUF 250

//===========================================================================
// GLOBALS
//===========================================================================

#define NBSP  0x00a0

/* NOTE: This is essentially the Unicode Consortium's mapping from
** ftp://ftp.unicode.org/Public/MAPPINGS/ETSI/GSM0338.TXT with the following
** exceptions:
**
** - UC feels that GSM 7-bit 0x09 should map to small letter c with cedilla
**   rather than capital letter c with cedilla.  We're sticking with our
**   previous mapping for backward compatibility.
*/
static const uint16 gsm7_to_utf16[] =
{
/* 0x00 */  '@',    0x00a3, '$',    0x00a5, 0x00e8, 0x00e9, 0x00f9, 0x00ec,
/* 0x08 */  0x00f2, 0x00c7, 0x000a, 0x00d8, 0x00f8, 0x000d, 0x00c5, 0x00e5,
/* 0x10 */  0x0394, '_',    0x03a6, 0x0393, 0x039b, 0x03a9, 0x03a0, 0x03a8,
/* 0x18 */  0x03a3, 0x0398, 0x039e, NBSP,   0x00c6, 0x00e6, 0x00df, 0x00c9,
/* 0x20 */  ' ',    '!',    0x0022, '#',    0x00a4, '%',    '&',    0x0027,
/* 0x28 */  '(',    ')',    '*',    '+',    ',',    '-',    '.',    '/',
/* 0x30 */  '0',    '1',    '2',    '3',    '4',    '5',    '6',    '7',
/* 0x38 */  '8',    '9',    ':',    ';',    '<',    '=',    '>',    '?',
/* 0x40 */  0x00a1, 'A',    'B',    'C',    'D',    'E',    'F',    'G',
/* 0x48 */  'H',    'I',    'J',    'K',    'L',    'M',    'N',    'O',
/* 0x50 */  'P',    'Q',    'R',    'S',    'T',    'U',    'V',    'W',
/* 0x58 */  'X',    'Y',    'Z',    0x00c4, 0x00d6, 0x00d1, 0x00dc, 0x00a7,
/* 0x60 */  0x00bf, 'a',    'b',    'c',    'd',    'e',    'f',    'g',
/* 0x68 */  'h',    'i',    'j',    'k',    'l',    'm',    'n',    'o',
/* 0x70 */  'p',    'q',    'r',    's',    't',    'u',    'v',    'w',
/* 0x78 */  'x',    'y',    'z',    0x00e4, 0x00f6, 0x00f1, 0x00fc, 0x00e0
};

//===========================================================================
// Function declarations
//===========================================================================

/*===========================================================================
  FUNCTION QM_UTIL_IS_IN_SERVICE()

  DESCRIPTION
    This function is used to check if a given RAT is in service.
    
  PARAMETERS
    subscription: subscription on which RAT to be checked in service
    rat: RAT which is to be checked for service
    p_info: pointer to the latest CM SS EVENT
   consider_limited_as_service: boolean value to indicate if LIMITED service on RAT to be considered as full service

  RETURN VALUE
    TRUE if the given RAT is in service, FALSE otherwise
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qm_util_is_in_service( enum qm_subs_e subscription, enum qm_rat_e rat, cm_mm_msim_ss_info_s_type *p_info, boolean consider_limited_as_service, enum qm_stack_index_e *stack )
{
  boolean retval = FALSE;

  if( stack != NULL )
  {
    *stack = STACK_INDEX_MAX;
  }
  

  switch(rat)
  {
    case QM_RAT_1X:
      if( p_info->stack_info[STACK_INDEX_0].sys_mode == SYS_SYS_MODE_CDMA && subscription == QM_SUBS_PRI )
      {
        if( p_info->stack_info[STACK_INDEX_0].srv_status == SYS_SRV_STATUS_SRV )
        {
          if( stack != NULL )
          {
            *stack = STACK_INDEX_0;
          }
          retval = TRUE;
        }
      }
      break;

    case QM_RAT_HDR:
      if( p_info->stack_info[STACK_INDEX_0].sys_mode == SYS_SYS_MODE_HDR && subscription == QM_SUBS_PRI )
      {
        if( p_info->stack_info[STACK_INDEX_0].srv_status == SYS_SRV_STATUS_SRV )
        {
          if( stack != NULL )
          {
            *stack = STACK_INDEX_0;
          }
          retval = TRUE;
        }
      }
      else if( p_info->number_of_stacks > 1 && p_info->stack_info[STACK_INDEX_1].sys_mode == SYS_SYS_MODE_HDR && subscription == QM_SUBS_PRI )
      {
        if(  p_info->stack_info[STACK_INDEX_1].srv_status == SYS_SRV_STATUS_SRV )
        {
          if( stack != NULL )
          {
            *stack = STACK_INDEX_1;
          }
          retval = TRUE;
        }
      }
      break;

    default:
      QM_MSG_ERROR_3("unhandled RAT %d, subs %d, consider_limited_as_service %d", rat, subscription, consider_limited_as_service);
      break;
  }

  return retval;
}

/*===========================================================================
  FUNCTION QM_UTIL_BCD16_TO_UINT16()

  DESCRIPTION
    Convert a BCD encoded 2/3 digit MCC/MNC stored in a uint16 to binary uint16 value.

  PARAMETERS
    bcd16     : binary value to convert
    is3digits : pointer to boolean, TRUE if 3 digits, FALSE if 2.
                A NULL may be passed if not interested.  Ex. for mcc.

  RETURN VALUE
    uint16 - The binary value of the 2 or 3 digit BCD number, or 
             0xFFFF if the bcd16 number was invalid for some reason.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None

  NOTES
     o Values stored in nibble 0x[0-9 F]
     o A 0xF in the LSN indicates the number is 2-digit instead of 3-digit.
     o A 0x0 should always stored in the MSN of the uint16.
     o Invalid if the LSN is in the inclusive range [ 0xA .. 0xE ], or
                  either of the middle two nibbles are > 0x9.

    |MSN|        LSN  Example: 3 digit mcc/mnc
    | 3 | 2 | 1 | 0 | Nibble
    |0x0|100| 10| 1 | Digit magnitude (nibble 3 should be zero, but not enforced).

    |MSN|        LSN  Example: 2 digit mnc
    | 3 | 2 | 1 | 0 | Nibble
    |0x0| 10| 1 | F | Digit magnitude (nibble 3 should be zero, but not enforced).

===========================================================================*/
uint16 qm_util_bcd16_to_uint16(
  uint16    bcd16,
  boolean * is3digits
)
{
  uint16 value = 0;
  uint8  nibble[3];            // nibble[0] least significant nibble, nibble[2] is most significant nibble of mcc/mnc
  uint8  i;

  for ( i = 0 ; i < sizeof(nibble) / sizeof(nibble[0]) ; i++ )
  {
    nibble[i] = ( bcd16 >> (i*4) ) & 0xF;

    if ( ( 9 < nibble[i] ) && ( nibble[i] < 0xF ) )
    {
      return QM_UTIL_INVALID_BCD16;
    }
    else if ( ( i != 0 ) && ( nibble[i] == 0xF ) )
    {
      return QM_UTIL_INVALID_BCD16;
    }
  }

  if ( nibble[0] != 0xF )
  {
    if ( is3digits != NULL) { *is3digits = TRUE; }
    value = (uint16) ( nibble[2] * 100 + nibble[1] * 10 + nibble[0] );
  }
  else
  {
    if ( is3digits != NULL) { *is3digits = FALSE; }
    value = (uint16) ( nibble[2] * 10 + nibble[1] );
  }

  if ( is3digits == NULL )
  {
    QM_MSG_MED_2("%u = qm_util_bcd_uint16_to_uint16( BCD16 = 0x%04X , NULL )", value, bcd16);
  }
  else
  {
    QM_MSG_MED_3("%u = qm_util_bcd_uint16_to_uint16( BCD16 = 0x%04X , %d (T/F) )", value, bcd16, *is3digits);
  }

  return value;
} //qm_util_bcd16_to_uint16()

/*===========================================================================
  FUNCTION QM_UTIL_MAP_RAT_ENUM_TO_BITMASK()

  DESCRIPTION
    This function maps rat enum to rat bitmask

  PARAMETERS
    rat: enum to convert to bitmask

  RETURN VALUE
    uint32: bitmask value for RAT.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
uint32 qm_util_map_rat_enum_to_bitmask( enum qm_rat_e rat )
{
  uint32 rat_bitmask_value = 0;

  switch ( rat )
  {
    case QM_RAT_1X:
      rat_bitmask_value = QM_RAT_MASK_C;
      break;

    case QM_RAT_HDR:
      rat_bitmask_value = QM_RAT_MASK_H;
      break;

    case QM_RAT_GERAN:
      rat_bitmask_value = QM_RAT_MASK_G;
      break;

    case QM_RAT_UMTS:
      rat_bitmask_value = QM_RAT_MASK_W;
      break;

    case QM_RAT_LTE:
      rat_bitmask_value = QM_RAT_MASK_L;
      break;

    case QM_RAT_TDSCDMA:
      rat_bitmask_value = QM_RAT_MASK_T;
      break;

    default:
      QM_MSG_HIGH_1("Unsupported rat %d", rat);
      break;
  }

  return rat_bitmask_value;
}

/*===========================================================================
  FUNCTION QM_UTIL_MAP_CM_SYS_MODE_TO_QM_BITMASK()

  DESCRIPTION
    This function maps cm sys_mode to qm rat bitmask

  PARAMETERS
    sys_mode: cm sys_mode to convert

  RETURN VALUE
    uint32: bitmask value for RAT.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
uint32 qm_util_map_cm_sys_mode_to_qm_bitmask( sys_sys_mode_e_type sys_mode )
{
  uint32 rat_bitmask_value = 0;

  switch ( sys_mode )
  {
    case SYS_SYS_MODE_CDMA:
      rat_bitmask_value = QM_RAT_MASK_C;
      break;

    case SYS_SYS_MODE_HDR:
      rat_bitmask_value = QM_RAT_MASK_H;
      break;

    case SYS_SYS_MODE_GSM:
      rat_bitmask_value = QM_RAT_MASK_G;
      break;

    case SYS_SYS_MODE_WCDMA:
      rat_bitmask_value = QM_RAT_MASK_W;
      break;

    case SYS_SYS_MODE_LTE:
      rat_bitmask_value = QM_RAT_MASK_L;
      break;

    case SYS_SYS_MODE_TDS:
      rat_bitmask_value = QM_RAT_MASK_T;
      break;

    default:
      QM_MSG_LOW_1("Unsupported rat %d", sys_mode);
      break;
  }

  return rat_bitmask_value;
}

/*===========================================================================
  FUNCTION QM_UTIL_MAP_CM_ASUBS_TO_QM_SUBS()

  DESCRIPTION
    This function maps cm asubs id to QM subs enum

  PARAMETERS
    asubs_id: cm asubs id to map to QM

  RETURN VALUE
    enum q_subs_e: enum for QM subscription.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
enum qm_subs_e qm_util_map_cm_asubs_to_qm_subs( sys_modem_as_id_e_type asubs_id )
{
  enum qm_subs_e subs_id = QM_SUBS_MIN;

  switch ( asubs_id )
  {
    case SYS_MODEM_AS_ID_1:
      subs_id = QM_SUBS_PRI;
      break;
#ifdef FEATURE_DUAL_SIM
    case SYS_MODEM_AS_ID_2:
      subs_id = QM_SUBS_SEC;
      break;
#endif
#ifdef FEATURE_TRIPLE_SIM
    case SYS_MODEM_AS_ID_3:
      subs_id = QM_SUBS_TER;
      break;
#endif
    default:
      QM_MSG_HIGH_1("Unhandled asubs_id %d", asubs_id);
      break;
  }

  return subs_id;
}

// Function only needed until all client information moves to nas_clnt module
/*===========================================================================
  FUNCTION QM_UTIL_MAP_NAS_SUBS_TO_QM_SUBS()

  DESCRIPTION
    This function maps QMI NAS subs enum to QM subs enum

  PARAMETERS
    subs_id: QMI_NAS subs enum to map to QM

  RETURN VALUE
    enum q_subs_e: enum for QM subscription.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
enum qm_subs_e qm_util_map_nas_subs_to_qm_subs( enum qmi_nas_subs_e subs_id )
{
  enum qm_subs_e qm_subs_id = QM_SUBS_MIN;

  switch ( subs_id )
  {
    case QMI_NAS_SUBS_PRIMARY:
      qm_subs_id = QM_SUBS_PRI;
      break;
#ifdef FEATURE_DUAL_SIM
    case QMI_NAS_SUBS_SECONDARY:
      qm_subs_id = QM_SUBS_SEC;
      break;
#endif
#ifdef FEATURE_TRIPLE_SIM
    case QMI_NAS_SUBS_TERTIARY:
     subs_id = QM_SUBS_TER;
     break;
#endif

    default:
      QM_MSG_HIGH_1("Unhandled subs_id %d", subs_id);
      break;
  }

  return qm_subs_id;
}


/*===========================================================================
FUNCTION qm_util_report_dog

DESCRIPTION
  This function reports to dog

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void qm_util_report_dog
(
  void
)
{
  dog_hb_report(qmi_mmode_dog_rpt_var);
} /* qm_util_report_dog */

/*===========================================================================
FUNCTION  QM_UTIL_DECODE_UCS2_80_81_82_ENCODING

  DESCRIPTION
  Extracts the UCS2 80, 81, or 82 encoded buffer and decodes into UCS2 little
  endian.

DEPENDENCIES
  None.

RETURN VALUE
  size_t len: Length of the output buffer, number of uint16 characters.
    
SIDE EFFECTS
  None
==============================================================================*/
size_t qm_util_decode_ucs2_80_81_82_encoding( uint8 *buf_in, 
                                             size_t buf_in_len,
                                             uint16 *buf_out, 
                                             size_t buf_out_max_len)
{
  uint8 i, j;
  uint8 *in_p = buf_in;
  uint16 *out_p16 = buf_out;
  uint16 ucs2_base = 0;
  uint8 ucs2_len = 0;
  size_t buf_len = 0;

  if ((buf_in == NULL) || (buf_out == NULL)) {
    return buf_len;
  }

  in_p++;
  /* The first octet, if 0x80, 0x81, 0x82 denotes the UCS2 encoding, else
   * the GSM encoding is used.
   */
  QM_MSG_HIGH_1("In qm_util_decode_ucs2_80_81_82_encoding : Incoming buffer encoding used 0x%02x", buf_in[0]);
  switch (buf_in[0]) 
  {
    case QM_UCS2_80:
      for (i=0,j=0; (i<(buf_out_max_len-1)) && (j < (buf_in_len-1)); i++, j+=2, in_p+=2, out_p16++) 
  {
        if (*in_p == 0xFF)
          break;
        else 
    {
          *out_p16 = *(in_p);
          *out_p16 <<= 8;
          *out_p16 |= *(in_p+1);
        }
    }
      if (i > 0) {
        buf_len = i;
  }
      break;

    case QM_UCS2_81:
      /* the second-octet is length of UCS2 chars in the EF-SPN */
      ucs2_len = *(in_p++);
      if ((ucs2_len > 0) && (ucs2_len < buf_out_max_len))
  {
        /* the third octet contains an 8-bit number which defines bits
         * 15 - 8 of a 16-base pointer, where bit 16 is set to 0, and bits 7 - 1 
         * are also set to zero. These 16 bits consitute a base pointer to a 
         * half-page in the UCS2 code space, to be used with some or all of the 
         * remaining octets in the string. 
         */
        ucs2_base = *(in_p++);
        ucs2_base <<= 7;
        
        for (i=0; (i < ucs2_len) && (i < buf_in_len-3); i++, in_p++, out_p16++) 
    {
          /*                                        UNICODE              :  GSM-7bit */
          *out_p16 = (*(in_p) & 0x80) ? ( ucs2_base + (*(in_p) & 0x7F) ) : *(in_p);
    }
        buf_len = i;
  }
      break;

    case QM_UCS2_82:
      ucs2_len = *(in_p++);
      if ((ucs2_len > 0) && (ucs2_len < buf_out_max_len)) 
  {
        /* the third and fourth octets contain a 16-bit number which defines the complete
         * 16-bit base pointer to a 'half-page' in the UCS2 code space, for use with some
         * or all of the remaining octets in the string. 
         */
        ucs2_base = *(in_p++);
        ucs2_base <<= 8;
        ucs2_base |= *(in_p++);

        for (i=0; (i < ucs2_len) && (i < buf_in_len-4); i++, in_p++, out_p16++) 
    {
          /*                                            UNICODE          :  GSM-7bit */
          *out_p16 = (*(in_p) & 0x80) ? ( ucs2_base + (*(in_p) & 0x7F) ) : *(in_p);
    }
        buf_len  = i;
  }
      break;

    default:
      QM_MSG_ERROR("Input buffer not encoded in UCS2 80,81 or 82");
      break;
  }

  return buf_len;
} /* qm_util_decode_ucs2_80_81_82_encoding */


/*===========================================================================
FUNCTION qm_util_read_efs

DESCRIPTION
  This function reads the EFS item from the item file path

DEPENDENCIES
  None

PARAMETERS
  item_file_path - File path to item in EFS
  efs_info_ptr - Struct for EFS item(s)
  efs_info_size - Size of EFS item structure

RETURN VALUE
   TRUE         - Success
 -FALSE         - Non-EFS related Failures

SIDE EFFECTS
  None
===========================================================================*/
boolean qm_util_read_efs
(
  const char      *item_file_path,
  void            *efs_info_ptr,
  uint32          efs_info_size
)
{
  int                         retval = 0;
  int                         fd;


  if ( NULL == item_file_path || NULL == efs_info_ptr )
  {
    QM_MSG_ERROR("item_file_path or nv_info_ptr error");
    return FALSE;
  }

  memset(efs_info_ptr, 0x00, efs_info_size);

  QM_MSG_HIGH_1("qm_util_read_efs:Reading from EFS, Open file=%s", item_file_path);

  /* Report to dog before efs access just to be safe */
  qm_util_report_dog();

  fd = mcfg_fopen(item_file_path, MCFG_FS_O_RDONLY, MCFG_FS_ALLPERMS, MCFG_FS_TYPE_EFS, MCFG_FS_SUBID_0);

  if ( fd < 0 )
  {
    QM_MSG_ERROR_1("mcfg_fopen error=%d", fd);

    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Read data from file.*/
  retval = mcfg_fread(fd, efs_info_ptr, (fs_size_t)efs_info_size, MCFG_FS_TYPE_EFS);

  if( retval < 0 )
  {
    QM_MSG_ERROR_1("efs read error %d", retval);
    
    (void) mcfg_fclose( fd, MCFG_FS_TYPE_EFS );
    return FALSE;
  }

   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Return the number of bytes actually read.*/
  QM_MSG_HIGH_1("EFS data size read=%d", retval);

  /* Close the file */
  (void) mcfg_fclose( fd, MCFG_FS_TYPE_EFS );

  return TRUE;
} /* qm_util_read_efs() */

/*===========================================================================
  FUNCTION QM_UTIL_PRINT_BYTES()

  DESCRIPTION
    This function prints out a byte string of a specified length

  PARAMETERS
    data_ptr: pointer to byte string to print
    data_len: length of byte string to print

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qm_util_print_bytes
(
   uint8  *data_ptr,
   uint32  data_len
)
{
  uint8 *out_buf, *out_ptr;
  boolean err = FALSE;

  if ( data_len == 0 )
  {
    QM_MSG_HIGH("Attempted to print 0 length buffer");
    return;
  }

  out_buf = modem_mem_alloc( QM_MAX_F3_BUF, MODEM_MEM_CLIENT_QMI_MMODE );

  if ( out_buf != NULL )
  {
    int i, sn_size, j = 0;

    while ( j < data_len )
    {
      out_ptr = out_buf;
      memset( out_buf, 0x00, QM_MAX_F3_BUF );

      for( i = 0; j < data_len && i+3 < QM_MAX_F3_BUF; j++ )
      {
        sn_size = snprintf( (char*)out_ptr, sizeof(out_ptr), " %X", data_ptr[j] );
        if ( sn_size > 0 )
        {
          out_ptr += sn_size;
          i += sn_size;
        }
        else
        {
          QM_MSG_ERROR("Error occured while attempting to print buffer");
          err = TRUE;
          break;
        }
      }

      if ( err )
      {
        break;
      }
      else
      {
        QM_MSG_LOW_1("%s", out_buf);
      }
    }

    modem_mem_free( out_buf, MODEM_MEM_CLIENT_QMI_MMODE );
  }
}

/*===========================================================================
  FUNCTION QM_UTIL_RETRIEVE_CSG_ID()

  DESCRIPTION
    This function is used to retrieve the CSG ID for a given stack.
    
  PARAMETERS
    p_info: pointer to the latest CM SS EVENT
    stack_index: stack to retrieve CSG ID for

  RETURN VALUE
    CSG ID if 3GPP stack, SYS_CSG_ID_INVALID if not
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
sys_csg_id_type qm_util_retrieve_csg_id( cm_mm_msim_ss_info_s_type *p_info, uint8 stack_index )
{
  sys_csg_id_type csg_id = SYS_CSG_ID_INVALID;

  if( stack_index < p_info->number_of_stacks )
  {
    switch( p_info->stack_info[stack_index].sys_mode )
    {
      case SYS_SYS_MODE_GSM:
        csg_id = p_info->stack_info[stack_index].rat_mode_info.gsm_mode_info.csg_info.csg_id;
        break;
        
      case SYS_SYS_MODE_WCDMA:
        csg_id = p_info->stack_info[stack_index].rat_mode_info.wcdma_mode_info.csg_info.csg_id;
        break;

      case SYS_SYS_MODE_LTE:
        csg_id = p_info->stack_info[stack_index].rat_mode_info.lte_mode_info.csg_info.csg_id;
        break;

      case SYS_SYS_MODE_TDS:
        csg_id = p_info->stack_info[stack_index].rat_mode_info.tds_mode_info.csg_info.csg_id;
        break;

      default:
        QM_MSG_ERROR_2("Invalid rat %d on stack %d for csg_id", p_info->stack_info[stack_index].sys_mode, stack_index);
        break;
    }
  }

  return csg_id;
}
/*=====================================================================*/
void qmi_voice_mem_fatal
(
  void
)
{
       QM_MSG_FATAL("Can't allocate memory");
}/* qmi_voice_mem_fatal */
/*=====================================================================*/
void qmi_voice_cmd_fatal
(
  void
)
{
       QM_MSG_FATAL("Out of QMI_MMODE cmd bufs");
}/* qmi_voice_cmd_fatal */

/*=====================================================================*/
void qmi_voice_mem_error
(
  void
)
{
      QM_MSG_ERROR("Can't allocate memory");
}/* qmi_voice_mem_error */

/*=====================================================================*/
#ifdef FEATURE_DUAL_SIM
void qmi_voice_asid
(
  qmi_voice_cm_if_info_item_s *const params_ptr
)
{
  uint8 as_id=QMI_VOICE_CM_IF_AS_ID_PRIMARY;
  as_id = params_ptr->header.as_id;
  QM_MSG_MED_1("Recieved asubs_id=%d", as_id);
}/* qmi_voice_asid */
#endif
/*=====================================================================*/
void qmi_nas_invalid_len_tlv
(
  void
)
{
    QM_MSG_ERROR("Invalid length in TLV");
}/* qmi_nas_invalid_len_tlv */
/*=====================================================================*/
void qmi_err_write
(
  uint8 resp_type,
  void* value
)
{
    QM_MSG_ERROR_2("Could not write type: %d, value: %x", resp_type, value);
}/* qmi_err_write */
/*=============================================================================
FUNCTION: qm_util_gsm7_to_utf16

DESCRIPTION:
   Converts an unpacked 7-bit GSM string to an ISO 8859-1 string.

PARAMETERS:
  gsm7_string       Unpacked GSM 7-bit string.
  gsm7_len          Number of bytes in gsm7_string.
  use_extension     If TRUE, use Basic Character Set Extension of 3GPP TS 23.038.
                    If FALSE, only return characters from the Basic Character Set.
  utf16_string      Pointer to buffer into which to convert to UTF-16
  utf16_len         Number of characters (*not* bytes) in buffer pointed to by
                    utf16_string.
  num_converted     Pointer to storage for number of characters that were
                    written to utf16_string (not counting the terminating NUL).

RETURN VALUE:
  CHR_CVRT_SUCCESS      - conversion was successful
  CHR_CVRT_TRUNCATED    - destination buffer was not large enough to hold
                          the converted string, so it was truncated
=============================================================================*/
uint16 qm_util_gsm7_to_utf16(
  const byte  *gsm7_string,     /* unpacked gsm 7-bit string */
  size_t      gsm7_len,         /* number of bytes in gsm7_string */
  boolean     use_extension,    /* should the GSM 7-bit extension table be used? */
  uint16      *utf16_string,    /* pointer to where to put converted string */
  size_t      utf16_len,        /* size of buffer pointed to by utf16_string */
  size_t      *num_converted_ptr/* pointer in which to store number of
                                   characters written to utf16_string */
  )
{
  size_t  num_utf16 = 0;
  uint16  result = CHR_CVRT_SUCCESS;
  size_t  i;
  uint16  utf16_char;

  QM_MSG_MED_3("In qm_util_gsm7_to_utf16(): gsm7_len=%d, use_extension=%d, utf16_len=%d", gsm7_len, use_extension, utf16_len);

  if(utf16_string == NULL)
  {
    QM_MSG_MED("Received NULL for utf16_string. Returning CHR_CVRT_TRUNCATED.");
    return CHR_CVRT_TRUNCATED;
  }
  else if(gsm7_string == NULL)
  {
    QM_MSG_MED("Received NULL for gsm7_string. Setting gsm7_len=0.");
    gsm7_len = 0;
  }

  /*
  ** Convert the string
  */
  for (i = 0 ; i < gsm7_len ; ++i)
  {
    utf16_char = gsm7_to_utf16[gsm7_string[i]];

    /* A non-breaking space (NBSP) indicates that the next character is
    ** escaped if we're using the GSM 7-bit extension table
    */
    if (NBSP == utf16_char && use_extension && i < gsm7_len - 1)
    {
      ++i;
      utf16_char = gsm7_to_utf16[gsm7_string[i]];
    }

    if (num_utf16 >= utf16_len - 1)
    {
      result |= CHR_CVRT_TRUNCATED;
      break;
    }

    utf16_string[num_utf16++] = utf16_char;
  }

  utf16_string[num_utf16] = 0;

  
  QM_MSG_MED_3("num_utf16=%d, result=%d, num_converted_ptr(!NULL): %d", num_utf16, result, (NULL != num_converted_ptr)?1:0);

  if (NULL != num_converted_ptr)
  {
    *num_converted_ptr = num_utf16;
  }
  
  return result;

}
