/*****************************************************************************
***
*** TITLE
***
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_gprs_freq_lists.c#1 $
***
*** DESCRIPTION
***
***  Contains functions related to decoding and managing frequency lists
***
*** Copyright (c) 2003-2013 Qualcomm Technologies, Inc.
*** All Rights Reserved.  QUALCOMM Proprietary.  Export of this technology or
*** software is regulated by the U.S. Government. Diversion contrary to U.S.
*** law prohibited.
***
*****************************************************************************/

/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#include "geran_variation.h"
#include "customer.h"  /* Includes FEATURE_... defines */
#include "rr_seg_load.h"
#include "rr_general.h"
#include "rr_gprs_defs.h"
#include "rr_sys_info_i.h"
#include "gprs_mem.h"
#include "rr_multi_sim.h"


/*****************************************************************************
***
***     Local #defines, Type Definitions & Variables
***
*****************************************************************************/

#define ORIG_ARFCN(x)                   ( (word)(*x & 0x01) << 9 | \
                                          (word)*(x+1) << 1 | \
                                          (word)(*(x+2) & 0x80) >> 7 )

#define F0(x)                           ((word)(*x & 0x04) >> 2)

#define MAX_NO_W                        64
#define BIT_MAP_0_FORMAT(x)             ((x & 0xC0) == 0x00 )
#define RANGE_FORMAT_1024(x)            ((((x >> 1) & 0x04) == 0x00) && (x & 0x80))
#define RANGE_FORMAT_512(x)             ((((x >> 1) & 0x07) == 0x04) && (x & 0x80))
#define RANGE_FORMAT_256(x)             ((((x >> 1) & 0x07) == 0x05) && (x & 0x80))
#define RANGE_FORMAT_128(x)             ((((x >> 1) & 0x07) == 0x06) && (x & 0x80))
#define VAR_BIT_MAP_FORMAT(x)           ((((x >> 1) & 0x07) == 0x07) && (x & 0x80))

typedef enum
{
  RANGE_1024        = 1024,
  RANGE_512         = 512,
  RANGE_256         = 256,
  RANGE_128         = 128
} freq_range_format_t;


/*****************************************************************************
***
***     Functions Prototypes
***
*****************************************************************************/

static uint8 decode_freq_list_bit_map_0(
  const byte * msg_ptr,
  uint8        freq_list_len,
  ARFCN_T  *   dest_ptr,
  uint8        max_no_freq
);

static uint16 greatest_power_of_2_lesser_or_equal_to( uint16 );

static uint16 get_w_from_freq_list( freq_range_format_t, byte *, uint16 *, uint8 );

static void flist_sort ( ARFCN_T *, uint16 );

static uint8 decode_freq_list_range_format_n ( freq_range_format_t, byte *, ARFCN_T *, uint8 );

static uint8 decode_freq_list_var_bit_map(
  const byte * msg_ptr,
  ARFCN_T *    freq_list_ptr,
  uint8        max_no_freq,
  uint8        freq_list_len
);


/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/

/*===========================================================================

FUNCTION rr_gprs_decode_freq_list_ie

DESCRIPTION
  Decodes the frequency list element (see 04.18 10.5.2.13).

PARAMS
  * IE - IE octets
    IE_len - number of octets in IE
  * freq_list - resulting frequency list

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rr_gprs_decode_freq_list_ie(
  byte *IE,
  uint8 IE_len,
  cell_channel_description_T *freq_list,
  const gas_id_t gas_id
)
{
  uint8 num_of_arfcns = 0;
  ARFCN_T *arfcn_ptr;
  uint8    index;

  if ( BIT_MAP_0_FORMAT(*IE) )
  {
    num_of_arfcns = decode_freq_list_bit_map_0( IE, IE_len, freq_list->channel_number, CA_MAX_LEN );
  }
  else
  if ( RANGE_FORMAT_1024(*IE) )
  {
    num_of_arfcns = decode_freq_list_range_format_n( RANGE_1024, IE, freq_list->channel_number, IE_len );
  }
  else
  if ( RANGE_FORMAT_512(*IE) )
  {
    num_of_arfcns = decode_freq_list_range_format_n( RANGE_512, IE, freq_list->channel_number, IE_len );
  }
  else
  if ( RANGE_FORMAT_256(*IE) )
  {
    num_of_arfcns = decode_freq_list_range_format_n( RANGE_256, IE, freq_list->channel_number, IE_len );
  }
  else
  if ( RANGE_FORMAT_128(*IE) )
  {
    num_of_arfcns = decode_freq_list_range_format_n( RANGE_128, IE, freq_list->channel_number, IE_len );
  }
  else
  if ( VAR_BIT_MAP_FORMAT(*IE) )
  {
    num_of_arfcns = decode_freq_list_var_bit_map( IE, freq_list->channel_number, CA_MAX_LEN, IE_len );
  }

  freq_list->no_of_entries = num_of_arfcns;
  arfcn_ptr = freq_list->channel_number;
  for (index = 0; index < num_of_arfcns; index++, arfcn_ptr++)
  {
    arfcn_ptr->band = (sys_band_T)rr_get_band(arfcn_ptr->num, gas_id);
  }
} /* rr_gprs_decode_freq_list_ie() */



/*===========================================================================

FUNCTION rr_gprs_decode_mobile_alloc_freq_list

DESCRIPTION
  Decodes the GPRS mobile allocation frequency list element (see 04.60 12.8).
  The data passed into the function is formatted as "direct encoding 2".

PARAMS
  uint8 * ma_freq_list - data stream formatted as "direct encoding 2"
  uint8 ma_freq_list_len - num of octets
  gprs_mobile_allocation_t * - pointer to resulting GPRS Mobile Allocation

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void rr_gprs_decode_mobile_alloc_freq_list(
  uint8 *ma_freq_list,
  uint8 ma_freq_list_len,
  gprs_mobile_allocation_t *mobile_alloc,
  const gas_id_t gas_id
)
{
  uint8 i;
  cell_channel_description_T *channel_description_ptr;

  channel_description_ptr = GPRS_MEM_MALLOC(sizeof(cell_channel_description_T));

  RR_NULL_CHECK_FATAL(channel_description_ptr);

  rr_gprs_decode_freq_list_ie(
    ma_freq_list,
    ma_freq_list_len,
    channel_description_ptr,
    gas_id
  );

  for ( i = 0; i < channel_description_ptr->no_of_entries; i++ )
  {
    mobile_alloc->arfcns[i] = channel_description_ptr->channel_number[i];
  }

  mobile_alloc->num_of_arfcns = i;

  GPRS_MEM_FREE(channel_description_ptr);

} /* end rr_decode_gprs_mobile_alloc_freq_list() */

/*===========================================================================

FUNCTION rr_gprs_decode_mobile_alloc

DESCRIPTION
  Decodes the GPRS mobile allocation element.
  The function is provided with the incoming element data, a storage area, and
  a channel description structure to place the resulting ARFCN list.
  The function can operate in several ways:
  a) Decode the GPRS MA element as part of an instance of a PSI2 message. In this
     case, it is not possible to produce a ARFCN list, as all PSI2 instances must
   be received before this is possible (in order to guarantee having a Cell
   Allocation and Reference Frequency Lists). Here, the storage area ('ma_store')
   is used to store the partially decoded data, ready for when the complete decode
   can be performed later.
   Only 'ma_data' and 'ma_store' are supplied; 'cell_alloc' and 'mobile_alloc' are NULL.
  b) Decode a one-off GPRS MA element, as received in a e.g. Downlink Packet Assignment
     message. In this case, a complete decode can be performed immediately, using
   the Cell Allocation and Reference Frequency Lists already received in a
   previous acquisition of PSI2. 'ma_store' is used as a temporary scratch-pad.
   'ma_data', 'ma_store', 'cell_alloc' and 'mobile_alloc' are all supplied.

PARAMS
  gprs_mobile_alloc_t * - pointer to incoming GPRS MA element
  gprs_mobile_allocation_ie_t * - pointer to storage area used during the decode process
  cell_channel_description_T * - pointer to resulting ARFCN list

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void rr_gprs_decode_mobile_alloc(
  const gprs_mobile_alloc_t *   ma_data,
  gprs_mobile_allocation_ie_t * ma_store,
  cell_channel_description_T *  cell_alloc,
  gprs_mobile_allocation_t *    mobile_alloc
)
{
  /* The Mobile Allocation defines a list of ARFCNs for use by the MS. This is done by */
  /* referencing the ARFCNs in the Cell Allocation. */
  /* The Mobile Allocation is defined by: */
  /* 1. supplying a bitmap, which indicates which of the ARFCNs within the ARFCN list */
  /*    should be included */
  /* 2. by copying the ARFCN list, and supplying a list of ARFCN indexes into that list */
  /*    wich should be excluded. (If no ARFCN index list is given, the whole ARFCN list */
  /*    is valid */

  /* The Cell Allocation, may not have been received yet, so the Mobile Allocation */
  /* ARFCNs cannot be determined yet */
  /* So just store the remainder of the data required, and process later */
  if ( ma_data )
  {
    if ( ma_data->ma_arfcn_flag == ma_list_valid )
    {
      ma_store->ma_bitmap_valid = TRUE;
      ma_store->ma_bitmap = ma_data->ma_bitmap;
    }
    else
    {
      uint8 i;

      ma_store->ma_bitmap_valid = FALSE;
      ma_store->num_of_arfcn_indexes = ma_data->size_of_arfcn_list;

      for ( i = 0; i < ma_data->size_of_arfcn_list; i++ )
      {
        ma_store->arfcn_indexes[i] = ma_data->arfcn_index_list[i];
      }
    }

    ma_store->hsn = ma_data->hsn;
  }

  /* If storage for the mobile allocation was supplied, assume there is enough data in */
  /* 'ma_store' to complete the decode */
  if ( mobile_alloc && cell_alloc )
  {
    uint8 rfl_index, ma_index;
    uint64 ma_bitmap;

    /* The ARFCN list for this MA_NUMBER could be specified by either: */
    /* a) A bitmap, upto 64 bits, with a bit set indicating an offset into the Cell Alloction */
    /* b) An optional list of ARFCNs to REMOVE from the Cell Allocation */

    /* If 'ma_bitmap_valid' is TRUE, then use method a) */
    if ( ma_store->ma_bitmap_valid && ( cell_alloc->no_of_entries > 0 ) )
    {
      ma_bitmap = ma_store->ma_bitmap;
      rfl_index = 0;  /* Count through source list of ARFCNs */
      ma_index = 0;   /* Count through dest storage of resulting ARFCN list */

      while ( ma_bitmap )
      {
        if ( ((ma_bitmap & 1) != 0) && (ma_index < MAX_NUM_OF_GPRS_MA_ARFCNS) )
        {
          mobile_alloc->arfcns[ma_index] = cell_alloc->channel_number[rfl_index];
          ma_index++;
        }

        ma_bitmap >>= 1;
        rfl_index++;
      }

      mobile_alloc->num_of_arfcns = ma_index;
    }
    else
    {
      ARFCN_T  rfl_arfcn;
      uint16   arfcn_index;
      boolean  exclude_flag;

      uint8  num_of_arfcn_indexes = ma_store->num_of_arfcn_indexes;

      ma_index = 0;   /* Count through dest storage of resulting ARFCN list */
      arfcn_index = 0;  /* Offset into list of ARFCN indexes */

      if ( num_of_arfcn_indexes > 0 )
      {
        for ( rfl_index = 0; rfl_index < cell_alloc->no_of_entries; rfl_index++ )
        {
          /* Assume the ARFCN will be copied from the RFL to the MA */
          exclude_flag = FALSE;

          /* The ARFCN to copy is the ARFCN in the RFL indexed by rfl_index */
          rfl_arfcn = cell_alloc->channel_number[rfl_index];

          /* Scan through the ARFCN index list, and if an index in the list matches the */
          /* current index into the RFL, ignore the ARFCN this RFL index refers to */
          for ( arfcn_index = 0; arfcn_index < num_of_arfcn_indexes; arfcn_index++ )
          {
            if ( rfl_index == ma_store->arfcn_indexes[arfcn_index] )
            {
              exclude_flag = TRUE;
              break;
            }
          }

          if ( exclude_flag == FALSE )
          {
            mobile_alloc->arfcns[ma_index] = rfl_arfcn;
            ma_index++;
          }
        }

        mobile_alloc->num_of_arfcns = ma_index;
      }
      else
      {
        for ( rfl_index = 0; rfl_index < cell_alloc->no_of_entries; rfl_index++ )
        {
          mobile_alloc->arfcns[rfl_index] = cell_alloc->channel_number[rfl_index];
        }

        mobile_alloc->num_of_arfcns = rfl_index;
      }
    }

    mobile_alloc->hsn = ma_store->hsn;
  }

} /* end rr_gprs_decode_mobile_alloc() */


/*===========================================================================

FUNCTION rr_gprs_conv_mobile_alloc_to_rrps_format

DESCRIPTION
  Takes a GPRS mobile allocation structure of type gprs_mobile_allocation_t,
  and converts it into one of type rrps_frequency_list

PARAMS
  gprs_mobile_allocation_t * new_mobile_alloc
  rrps_frequency_list_t * store
  boolean * compare - TRUE if comparison required; NULL or FALSE otherwise
                      Result of comparison placed here - FALSE if a mis-match;

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void rr_gprs_conv_mobile_alloc_to_rrps_format(
  const gprs_mobile_allocation_t * new_mobile_alloc,
  rrps_frequency_list_t *          store
)
{
  uint8 i;

  for ( i = 0; i < new_mobile_alloc->num_of_arfcns; i++ )
  {
    CONVERT_TO_ARFCN_T( store->ma_number[i], new_mobile_alloc->arfcns[i] );
  }

  store->list_length = i;
  store->hsn = new_mobile_alloc->hsn;

} /* end rr_gprs_conv_mobile_alloc_to_rrps_format() */


/*****************************************************************************
***
***     Local Functions
***
*****************************************************************************/

/*===========================================================================

FUNCTION decode_freq_list_bit_map_0

DESCRIPTION
  Decodes a frequency list IE in "bitmap 0" format

PARAMS
  None

RETURN VALUE
  uint8 - number of frequencies decoded

SIDE EFFECTS
  None

===========================================================================*/

static uint8 decode_freq_list_bit_map_0(
  const byte * msg_ptr,
  uint8        freq_list_len,
  ARFCN_T  *   dest_ptr,
  uint8        max_no_freq
)
{
  uint16  arfcn_no;
  uint8   bit_mask;
  uint8   act_freq_count;
  uint8   byte_ctr;
  uint8   bit_counter;

  act_freq_count = 0;
  arfcn_no = 0;

  for ( byte_ctr = (freq_list_len - 1); byte_ctr > 0; byte_ctr-- )
  {
    bit_mask = 0x01;

    for ( bit_counter = 0; bit_counter < 8; bit_counter++ )
    {
      arfcn_no++;

      if  ( ( *(msg_ptr + byte_ctr) & bit_mask) != 0 )
      {
        if ( act_freq_count < max_no_freq )
        {
          (dest_ptr + act_freq_count)->num = arfcn_no;
          act_freq_count++;
        }
        else
        {
          MSG_GERAN_HIGH_0("Num of freqs exceeded");

          return act_freq_count;
        }
      }

      bit_mask = (uint8)(bit_mask << 1);
    }
  }

  bit_mask = 0x01;

  for ( bit_counter = 0; bit_counter < 4;  bit_counter++ )
  {
    arfcn_no++;

    if  ( ( *(msg_ptr + byte_ctr) & bit_mask) != 0 )
    {
      if ( act_freq_count < max_no_freq )
      {
        (dest_ptr + act_freq_count)->num = arfcn_no;
        act_freq_count++;
      }
      else
      {
        MSG_GERAN_HIGH_0("Num of freqs exceeded");

        return act_freq_count;
      }
    }

    bit_mask = (uint8)(bit_mask << 1);
  }

  return act_freq_count;

} /* end decode_freq_list_bit_map_0 */



/*===========================================================================

FUNCTION greatest_power_of_2_lesser_or_equal_to

DESCRIPTION
  This function calculates the greater power of 2 lesser or equal to a given
  integer value.

PARAMS
  None

RETURN VALUE
  uint16

SIDE EFFECTS
  None

===========================================================================*/

static uint16 greatest_power_of_2_lesser_or_equal_to( uint16 i )
{
  uint16 power_2 = 0x01;

  while ( i >= (power_2 << 1) )
  {
    power_2 = (uint16)(power_2 << 1);
  }

  return power_2;

} /* end greatest_power_of_2_lesser_or_equal_to() */



/*===========================================================================

FUNCTION get_w_from_freq_list

DESCRIPTION
  This function fetches the W(i) elements from the frequency list IE

PARAMS
  None

RETURN VALUE
  uint16

SIDE EFFECTS
  None

===========================================================================*/

static uint16 get_w_from_freq_list( freq_range_format_t    range,
                                    byte                 * freq_list_ptr,
                                    uint16               * w,
                                    uint8                  len )
{
  byte   * end_of_freq_list_ie;
  uint8    bit_len;
  byte     bit_mask_1;
  uint8    i, k;

  uint8  t = 1;

  /* 'freq_list_ptr' points to "Length of Frequency List Contents */

  end_of_freq_list_ie = freq_list_ptr + len;

  switch (range)                /* specific initialisations     */
  {
    case RANGE_1024:
      bit_len           = 10;           /* max. number of bits  */
      bit_mask_1        = 0x02;         /* first Bit of W(1)    */
      w[0] = F0(freq_list_ptr);         /* ARFCN 0 used ?       */
      break;

    case RANGE_512:
      bit_len           = 9;            /* max. number of bits  */
      bit_mask_1        = 0x40;         /* first Bit of W(1)    */
      w[0] = ORIG_ARFCN(freq_list_ptr); /* W(0) = Orig. ARFCN   */
      break;

    case RANGE_256:
      bit_len           = 8;            /* max. number of bits  */
      bit_mask_1        = 0x40;         /* first Bit of W(1)    */
      w[0] = ORIG_ARFCN(freq_list_ptr); /* W(0) = Orig. ARFCN   */
      break;

    case RANGE_128:
      bit_len           = 7;            /* max. number of bits  */
      bit_mask_1        = 0x40;         /* first Bit of W(1)    */
      w[0] = ORIG_ARFCN(freq_list_ptr); /* W(0) = Orig. ARFCN   */
      break;

    default:
      MSG_GERAN_ERROR_1("Unknown freq format (%d)",range);
      return 0;
  }

  if ( RANGE_1024 != range )
  {
    freq_list_ptr += 2;                 /* skip Orig-ARFCN      */
  }

  for ( k = 1; freq_list_ptr < end_of_freq_list_ie; k++ )
  {
    if ( k == MAX_NO_W )
    {
      MSG_GERAN_HIGH_0("Num of freqs exceeded");

      return (k-1);
    }
    else
    {
      w [ k ] = 0;

      for ( i = 0; i < bit_len; i++ )
      {
        if ( *freq_list_ptr & bit_mask_1 )
        {
          w[ k ] =  w[ k ] | 0x01;        /* set bit n of W(k)    */
        }

        if ( (int8)i < (int8)(bit_len-1) )
        {
          w[ k ] = (uint16)(w[ k ] << 1);          /* don't shift at last bit */
        }

        bit_mask_1 = bit_mask_1 >> 1;

        if ( !bit_mask_1 )
        {
          /* current byte is not last byte of message ? */
          if ( (freq_list_ptr+1) < end_of_freq_list_ie )
          {
            bit_mask_1 = 0x80;
            freq_list_ptr++;
          }
          else
          /* last W value completely decoded */
          if ( (i+1) == bit_len && w[ k ] != 0 )
          {
            return (k);
          }
          else
          /* last W value incomplete */
          {
            return (k-1);
          }
        }
      }

      if ( w[ k ] == 0 )
      {
        return (k-1);
      }

      if ( k == t )      /* t = 1,3,7,15,31,.... */
      {
        t++;
        t = (uint8)(t << 1) - 1;
        bit_len--;
      }
    }
  }

  return (k-1);

} /* end get_w_from_freq_list */



/*===========================================================================

FUNCTION flist_sort

DESCRIPTION
  Sort the frequency list

PARAMS
  IN/OUT : f - array of arfcns
  IN     : n - num of arfcns in array

RETURN VALUE
  uint16

SIDE EFFECTS
  None

===========================================================================*/

static void flist_sort( ARFCN_T * f, uint16 n )
{
  uint16    i, j, lmin;
  ARFCN_T   temp;
  ARFCN_T  * v;

  ARFCN_T  * u = f;
  ARFCN_T  * w = f;

  lmin = u->num;

  for ( i = 0; i < n; i++ )
  {
    v = u;

    for ( j = i; j < n; j++ )
    {
      if ( v->num < lmin )
      {
        w = v;
        lmin = w->num;
      }

      v++;
    }

    if ( u->num > lmin )
    {
      temp = *u;
      *u = *w;
      *w = temp;
    }

    lmin = (++u)->num;
  }

} /* end flist_sort */



/*===========================================================================

FUNCTION decode_freq_list_range_format_n

DESCRIPTION
  This function decodes the a frequency list with range 1024, 512, 256 and 128
  format (see GSM 04.18 10.5.2.13).

PARAMS
  IN:  range format, e.g. 1024
  IN:  frequency IE
  OUT: set of frequencies F(i)
  IN:  length of IE

RETURN VALUE
  uint16

SIDE EFFECTS
  None

===========================================================================*/

static uint8 decode_freq_list_range_format_n( freq_range_format_t    range_format,
                                              byte                 * msg_ptr,
                                              ARFCN_T              * f,
                                              uint8                  len )
{
  uint16  k;
  uint16  index;
  uint16  w_size;
  uint16  n;
  uint16  range;
  uint16  gp;
  uint16  orig_range = (uint16)range_format - 1;
  uint16 *w;

  // Allocate storage for temporary 'w' array
  w = GPRS_MEM_MALLOC(sizeof(uint16) * MAX_NO_W);

  RR_NULL_CHECK_FATAL(w);

  w_size = get_w_from_freq_list( range_format, msg_ptr, w, len );

  /* ORIG_ARFCN */
  f[0].num = w[0];

  for ( k = 1; (k <= w_size) && (k < MAX_NO_W); k++ )
  {
    index = k;
    range = orig_range / greatest_power_of_2_lesser_or_equal_to( index );
    n = w[index];

    while ( index > 1 )
    {
      gp = greatest_power_of_2_lesser_or_equal_to( index );
      range = (uint16)(range << 1) + 1;

      if ( (2 * index) < (3 * gp ) )
      {
        index -= gp / 2;
        n += w[index] + range / 2;
      }
      else
      {
        index -= gp;
        n += w[index];
      }

      if ( n > range )
      {
        n -= range;
      }
    }

    if ( orig_range == 1023 )
    {
      if ( n > orig_range )
      {
        n -= orig_range;
      }
    }
    else
    {
      n = (n + w[0]) % 1024;
    }

    f[k].num = n;
  }

  if ( w[0] && ( range_format == RANGE_1024 ) )
  {
                                        /* if F0 is used    */
      /*for (n = 0; n < k; n++) f[n]--;*/
      f[0].num--;
  }

  if ( !w[0] && ( range_format == RANGE_1024 ) )
  {
    for ( n = 0; n < (k-1); n++ )
    {
      f[n].num = f[n+1].num;    /* shift left   */
    }
  }
  else
  {
    k++;
  }

  flist_sort( f, (uint16)(k-1) );

  GPRS_MEM_FREE(w);

  return (uint8)(k-1);

} /* end decode_freq_list_range_format_n */


/*===========================================================================

FUNCTION get_w_from_freq_list

DESCRIPTION
  This function fetches the W(i) elements from the frequency list IE

PARAMS
  *msg_ptr - pointer to freq list IE
  *freq_list_ptr - pointer to array of arfcns
   max_no_freq - maximum number of frequencies
   freq_list_len - length of the freq list IE (num of octets)

RETURN VALUE
  uint16 - number of arfcns decoded

SIDE EFFECTS
  None

===========================================================================*/

static uint8 decode_freq_list_var_bit_map(
  const byte * msg_ptr,
  ARFCN_T *    freq_list_ptr,
  uint8        max_no_freq,
  uint8        freq_list_len
)
{
  byte    bit_mask;
  uint8   i;
  uint16  k;
  uint16  N;
  uint16  orig_arfcn;
  uint8   no_freq;

  no_freq = 0;  /* num of frequencies */

  orig_arfcn = ORIG_ARFCN( msg_ptr );
  freq_list_ptr->num = orig_arfcn;
  freq_list_ptr++;
  no_freq++;

  /* RRFCN 1 - RRFCN 7 */

  msg_ptr += 2;
  bit_mask = 0x40;

  for ( N = 1; N <= 7; N++ )
  {
    if ( *msg_ptr & bit_mask )
    {
      if ( no_freq <= max_no_freq )
      {
        freq_list_ptr->num = ( orig_arfcn + N ) % 1024;
        freq_list_ptr++;
        no_freq++;
      }
      else
      {
        MSG_GERAN_HIGH_0("Num of freqs exceeded");

        return no_freq;
      }
    }

    bit_mask = bit_mask >> 1;
  }

  /* RRFCN 8 - RRFCN n */

  msg_ptr++;
  freq_list_len -= 3;

  for ( i = 0; i < freq_list_len; i++ )
  {
    bit_mask = 0x80;

    for ( k = 0; k < 8; k++, N++ )
    {
      if ( *msg_ptr & bit_mask )
      {
        if ( no_freq <= max_no_freq )
        {
          freq_list_ptr->num = ( orig_arfcn + N ) % 1024;
          freq_list_ptr++;
          no_freq++;
        }
        else
        {
          MSG_GERAN_HIGH_0("Num of freqs exceeded");

          return no_freq;
        }
      }

      bit_mask = bit_mask >> 1;
    }

    msg_ptr++;
  }

  return no_freq;

} /* end decode_freq_list_var_bit_map */



/* EOF */

