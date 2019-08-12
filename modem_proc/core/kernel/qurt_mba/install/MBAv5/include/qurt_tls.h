#ifndef QURT_TLS_H
#define QURT_TLS_H
/*=============================================================================

                 qurt_tls.h -- H E A D E R  F I L E

GENERAL DESCRIPTION
    Prototypes of TLS APIs 
EXTERNAL FUNCTIONS
   None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

      Copyright (c) 2009
                    by Qualcomm Technologies Incorporated.  All Rights Reserved.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.


$Header: //components/rel/core.mpss/3.5.c12.3/kernel/qurt_mba/install/MBAv5/include/qurt_tls.h#1 $ 
$DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
03/03/11   op      Add header file
=============================================================================*/

/*=============================================================================
												FUNCTIONS
=============================================================================*/
/**
 * Description: Initialize mutex used by TLS
 */
void qurt_tls_init(void);

/**
 * Description: Allocates a TLS key
 *
 * @param 	*key	- the allocated key value
 *					*destructor - callback function when a thread exits.
 * @return	EOK  - SUCCESS
 * @return	E_TLS_NOAVAIL - No free TLS key available
 */
int qurt_tls_create_key (int *key, void (*destructor)(void *));

/**
 * Description: Sets a value in TLS indexed by key
 *
 * @param   key   - Operated TLS key
 *          value - data to save at the specified key location.
 * @return  EOK - SUCCESS
 * @return  EINVALID - Not a valid key
 */
int qurt_tls_set_specific (int key, const void *value);

/**
 * Description: Gets a value in TLS indexed by key
 *
 * @param   key   - Operated TLS key
 * @return  value saved in TLS indexed by key
 * @return  0 (NULL) if key out of range
 */
void *qurt_tls_get_specific (int key);


/**
 * Description: Delete a TLS key
 *
 * @param   key  - Free the allocated key
 * @return  EOK - SUCCESS
 * @return  E_TLS_NOENT - The key is not already free
 */
int qurt_tls_delete_key (int key);

#endif /* QURT_TLS_H */
