/*===========================================================================

              GPRS V42 bis Data compression Module

DESCRIPTION
  This file contains the functionality of V.42bis that is performance critical
  and should be placed in IRAM if possible. The file is a modified version of
  the dsgcsdv42_iram.c used in GSM Circuit Switched V42 bis implementation.

Copyright (c) 1991 - 2013 Qualcomm Technologies, Inc.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath:  L:/src/asw/GSM/GCSD/vcs/dsgcsdv42c.c_v   1.3   21 Mar 2002 10:58:10   trebman  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gsndcp/src/gsnv42_iram.c#1 $ $DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03-07-16   tjw     Prevent compiler warning C3040E: no external declaration in translation unit
01/30/03   tmr     Initial release

===========================================================================*/

/*===========================================================================
Header files
===========================================================================*/

#include "geran_variation.h"
#include "customer.h" /* This include must be first before #ifdef FEATURE_  */

#ifdef FEATURE_GSM_GPRS_SNDCP_DCOMP

#include "comdef.h"
#include "gsni.h"
#include "gsnv42.h"
#include "err.h"


/* hash function used to give even distribution of dictionary entries */
#define GSN_HASHFUNC(prefix,suffix, hash_table_size) \
   ( ((((prefix + suffix) << 6) - prefix) + suffix) & (hash_table_size - 1))

#define GSN_INC_HASH_INDEX(index, hash_table_size) {index++; index &= (hash_table_size - 1);}


/****************************************************************************
 * Function name:  find match
 * -------------
 *
 * Description:    This routine searches the codetable using the hash table
 * -----------
 *
 * Returns:        returns hashtable location with the match or the 1st
 * -------         available space in the ds_v42_c_hash_table
 *
 ****************************************************************************/

word  gsn_v42_find_match
(
   uint32   codeword,
   uint32   character,
   word     *ds_v42_hash_ptr,
   word     *ds_v42_prefix_ptr,
   uint8    *ds_v42_app_char_ptr,
   uint8    *ds_v42_refill_hash_flag,
   uint8    *ds_v42_search_success,
   uint16   hash_table_size
)

{
   register uint32 index;
   register uint32 hash_location;

   register word   *tmp_hash_ptr       = ds_v42_hash_ptr;
   register word   *tmp_prefix_ptr     = ds_v42_prefix_ptr;
   register uint8  *tmp_app_char_ptr   = ds_v42_app_char_ptr;
   register byte   rehash              = 0;
   register word   first_deletion      = NULL_LOCATION;
   register word   rehash_deletions    = 0;


   /* locates likely hash table location */
   hash_location = GSN_HASHFUNC(codeword, character, hash_table_size);


/*lint -e(716) Info -- while(1) ... */
   while (1)      /* loop will exit when a match is found or an
                   * empty location */
   {
      index = tmp_hash_ptr[hash_location];   /* leave unused marker */

      if (index == V42BIS_UNUSED_MASK)
      {
         /* no match */
         //GCSD_DEBUG_MSG_2 ("encoder->find match: no match, code %d, char %d \n",codeword, character);

         /* return first insertion point */
         if (first_deletion != NULL_LOCATION)
         {
            hash_location = first_deletion;
         }

         if (rehash > 0)
         {
            /* if there are more rehashes than the #defined threshold
             * set flag to ensure the hash table will be defragged on
             * next call */
            if (rehash_deletions > REFILL_THRESHOLD)
            {
               *ds_v42_refill_hash_flag = TRUE;
            }
         }

         *ds_v42_search_success = FALSE;
         /* return location in hash table of the empty space where the codeword
          * and character should be added if necessary. This saves having
          * to search the hashtable again when calling 'add_string' later in the
          * main function  */
         return ((word)hash_location);

      }

      /* check entry is not a deletion */
      if (index != V42BIS_DELETION_MASK)
      {
         /* if not then check to see if the location matches the code and
          * character combination being searched for */
         if ( (tmp_prefix_ptr[index] == codeword) &&
              (tmp_app_char_ptr[index] == character) )
         {

            //GCSD_DEBUG_MSG_2 ("Match found, code %d,char %d\n",codeword, character);

            /* match found */



            *ds_v42_search_success = TRUE;
            /* return location in hash table of match */
            return ((word)hash_location);

         }


      }


      else      /* Marked Deletion */
      {
         /* This is a potential insertion point if a match is not found so
          * it is stored provided one has not already been found prior */
         if (first_deletion == NULL_LOCATION)
         {
            first_deletion = (word)hash_location;
         }



         rehash_deletions++;   /* only increment this
                                * counter for deletions */

      }

      /* rehash */
      rehash++;

      /* linear probe */
      GSN_INC_HASH_INDEX (hash_location, hash_table_size);


   }         /* end of while */
}



/****************************************************************************
 * Function name:  add string
 * -------------
 *
 * Description:    This routine adds an entry to the codetable and hashtable
 * -----------     it will also free up if necessary a space for the next
 *                   entry
 *
 * Returns:        none
 * -------
 ****************************************************************************/

void  gsn_v42_add_string
(
   uint32   hash_index,
   uint32   codeword,
   uint32   app_char,
   uint32   ds_v42_max_code,
   word     *ds_v42_next_code,
   word     *ds_v42_hash_ptr,
   word     *ds_v42_prefix_ptr,
   uint8    *ds_v42_app_char_ptr,
   uint8    *ds_v42_leaf_count_ptr,
   uint16   hash_table_size
)
{
   register uint32 index;
   register word   leafcode;

   register word   *tmp_hash_ptr       = ds_v42_hash_ptr;
   register word   *tmp_prefix_ptr     = ds_v42_prefix_ptr;
   register uint8  *tmp_app_char_ptr   = ds_v42_app_char_ptr;
   register uint8  *tmp_leaf_count_ptr = ds_v42_leaf_count_ptr;
   register uint32 max_code            = ds_v42_max_code;
   register uint32 next_code           = *ds_v42_next_code;

   //GCSD_DEBUG_MSG_2 ("enc->add str: attempt to add str %d char %c \n", codeword, app_char);


   /* Add entry */
   /* to hash table */
   tmp_hash_ptr[hash_index] = (word) next_code;

   /* to codetable */
   tmp_prefix_ptr[next_code] = (word) codeword;
   tmp_app_char_ptr[next_code] = (uint8) app_char;
   tmp_leaf_count_ptr[next_code] = 0;   /* reset number of leaves from this
                                     * node */


   /* increment leaf counter on previous node if the codeword is not just
   ** an 8-bit character
   */

   if (codeword & 0xFFFFFF00)
   {
      tmp_leaf_count_ptr[codeword] += 1;   /* increment previous nodes
                                        * leaf counter to represent
                                        * new addition */
   }


   /***** recover dictionary entry *****/
   /*
    * loop through codetable to find either empty location or a leaf
    * node
    */
/*lint -e(716) Info -- while(1) ... */
   while (1)
   {
      next_code++;

      if (next_code > max_code)
      {
         next_code = FIRST_CODE;
      }


      if (tmp_prefix_ptr[next_code] == V42BIS_UNUSED_MASK)
      {
         /* empty location */
         /*
          * ds_v42_c_next_code is now pointing to an empty space so no
          * need to continue
          */
         /* Save the next_code value before exiting */
         *ds_v42_next_code = (word) next_code;
         return;
      }


      /* check for a leaf node */
      if (tmp_leaf_count_ptr[next_code] == 0)
      {
         break;
      }

   }         /* end while */


   /*
    * Remove Leaf from Dictionary
    */


   /* find leaf in the hash table */
   leafcode = tmp_prefix_ptr[next_code];

   index = GSN_HASHFUNC(leafcode, tmp_app_char_ptr[next_code], hash_table_size);

   while (tmp_hash_ptr[index] != next_code)
   {
      GSN_INC_HASH_INDEX(index, hash_table_size);
   }


   /* decrement parent node(if there is one) leaf counter */
   if (leafcode & 0xFF00)
   {
      tmp_leaf_count_ptr[leafcode] -= 1;
   }

   tmp_prefix_ptr[next_code] = V42BIS_UNUSED_MASK;




   /* free hashtable space if possible */


   /* if next location is empty */
   if (tmp_hash_ptr[ ((index + 1) & (hash_table_size - 1)) ] == V42BIS_UNUSED_MASK)
   {
      tmp_hash_ptr[index] = V42BIS_UNUSED_MASK;

      /* free hash locations above deleted location if possible */
/*lint -e(716) Info -- while(1) ... */
      while (1)
      {
         if (index == 0)
            index = hash_table_size-1;
         else
            index--;

         /*
          * If the location above is marked deleted then mark
          * unused
          */
         if (tmp_hash_ptr[index] == V42BIS_DELETION_MASK)
         {
            tmp_hash_ptr[index] = V42BIS_UNUSED_MASK;
         }
         else
            break;
      }
   }

   /*
    * otherwise do a lazy delete * by marking the space as delete
    */
   else
   {
      tmp_hash_ptr[index] = V42BIS_DELETION_MASK;
   }

   /* Save the next_code value before exiting */
   *ds_v42_next_code = (word) next_code;
}


/****************************************************************************
 *
 * Function name:  refill hash
 * -------------
 *
 * Description:    this routine defrags the hash table to improve effeciency
 * -----------
 *
 * Returns:        none
 * -------
 *
 ****************************************************************************/

void  gsn_v42_refill_hash
(
   uint32   ds_v42_max_code,
   word     *ds_v42_hash_ptr,
   word     *ds_v42_prefix_ptr,
   uint8    *ds_v42_app_char_ptr,
   uint16   hash_table_size
)
{
   register uint32 index;
   register uint32 hash_index;

   register word   *tmp_hash_ptr     = ds_v42_hash_ptr;
   register word   *tmp_prefix_ptr   = ds_v42_prefix_ptr;
   register uint8  *tmp_app_char_ptr = ds_v42_app_char_ptr;
   register uint32 max_code          = ds_v42_max_code;

   /* clear hash table */
   MSG_GERAN_LOW_1("GSN ""Defragging hash table %x",tmp_hash_ptr);

   index = hash_table_size;
   while (index--)
   {
      *tmp_hash_ptr++ = V42BIS_UNUSED_MASK;
   }

   // Reset tmp_hash_ptr and index;
   tmp_hash_ptr = ds_v42_hash_ptr;
   index = FIRST_CODE;

   /* loop through codetable adding a hash entry for each location */
   while (index < max_code)
   {
      /* only had non empty codetable locations */
      if (tmp_prefix_ptr[index] != V42BIS_UNUSED_MASK)
      {

         hash_index = GSN_HASHFUNC(tmp_prefix_ptr[index], tmp_app_char_ptr[index], hash_table_size);
         /* loop through hash table until non-empty location is found */
         while (tmp_hash_ptr[hash_index] != V42BIS_UNUSED_MASK)
         {
            GSN_INC_HASH_INDEX(hash_index, hash_table_size);
         }

         tmp_hash_ptr[hash_index] = (word)index;
      }
      index++;
   }
}


/****************************************************************************
 *
 * Function name:   initialise tables
 * -------------
 *
 * Description:    this routine inits both hash and code tables
 * -----------
 *
 * Returns:        none
 * -------
 *
 ****************************************************************************/

void  gsn_v42_init_tables
(
   word     *ds_v42_hash_table,
   word     *ds_v42_prefix_table,
   uint16   hash_table_size,
   uint16   code_table_size

)
{
   register uint32 index;

   register word   *tmp_hash_table_ptr    = ds_v42_hash_table;
   register word   *tmp_prefix_table_ptr  = ds_v42_prefix_table;


   /*** init arrays ***/
   /* code table */

   for (index = 0; index < code_table_size; index++)
   {
      *tmp_prefix_table_ptr++ = V42BIS_UNUSED_MASK;
   }

   /* hash table */
   for (index = 0; index < hash_table_size; index++)
   {
      *tmp_hash_table_ptr++ = V42BIS_UNUSED_MASK;
   }

}

#else
void feature_gsm_gprs_iram_dummy(void) {}    /* To prevent compiler warning C3040E: no external declaration in translation unit */
#endif /* FEATURE_GSM_GPRS */


