#ifndef COREAPI_VARIATION_H
#define COREAPI_VARIATION_H

/* As an interim step to removing the dependency of the Core API on customer.h, the core API files have
   been modified to include this file instead, which will temporarily include customer.h until we
   have completed the cleanup effort.  When we are ready to break this dependency we can now modify
   this file to directly define any legacy features (if necessary), or simply be empty.
 
   If we are able to make this file be empty then Core can stop including this file at any time
   after that, which is much easier to manage.
*/

#include <customer.h>

#endif /* COREAPI_VARIATION_H */
