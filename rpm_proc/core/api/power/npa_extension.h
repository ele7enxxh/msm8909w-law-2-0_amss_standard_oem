#ifndef NPA_EXTENSION_H
#define NPA_EXTENSION_H

/*==============================================================================
  @file npa_extension.h

  NPA extension API 

  An optional convention for defining user user_data field for an
  arbitrary number of extensions.  

  Note: If you have a single pice of user data, or no user data at
  all, this extension mechanism can be ignored

  Copyright (c) 2009 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
============================================================================*/

/* NPA Extension Enumerations */
enum
{
  NPA_EXTENSION_USER_DATA,      /* Arbitrary user data */
  NPA_EXTENSION_REMOTE_RESOURCE,
  NPA_EXTENSION_FRAMEWORK_END = 0x3fffffff

  /* Custom user extension ids should be after
   * NPA_EXTENSION_FRAMEWORK_END. They do not need to be added to this
   * enum */
};

/* Header for an extension */
typedef struct npa_extension_header
{
   void   *next_extension;
   uint32  extension_id;
} npa_extension_header;


/* An extension is an arbitrary data structure that begins with an
 * npa_extension_header  */

/* 
   @brief npa_find_extension - Given a pointer to an extension
   structure, walk the list and return the first extension matching
   the given extension id. Returns NULL if no match is found

   It is the responsibility of the caller to only invoke this function
   on a well formed list of extensions.  Behavior is undefiend in the
   list is not well formed.

   The list is not locked during the search.
*/  
npa_user_data npa_find_extension( npa_extension_header *extension, 
                                  unsigned int          extension_id  );


#endif /* NPA_EXTENSION_H */
