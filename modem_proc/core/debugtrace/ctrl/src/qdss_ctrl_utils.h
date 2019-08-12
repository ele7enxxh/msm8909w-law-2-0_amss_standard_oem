#ifndef QDSS_CTRL_UTILS_H
#define QDSS_CTRL_UTILS_H

/*=============================================================================

FILE:         qdss_ctrl_utils.h

DESCRIPTION:  

================================================================================
              Copyright (c) 2013 Qualcomm Technologies Incorporated.
                         All Rights Reserved.
                QUALCOMM Proprietary and Confidential
==============================================================================*/

#define TRY(nErr, func) \
   if (0 != (nErr = (func))) {\
      goto nErr##bail; \
   }

#define THROW(nErr, errno) \
   nErr = errno; \
   goto nErr##bail;

#define THROW_IF(nErr, condition, errno)        \
   if (condition) {                             \
      nErr = errno;                             \
      goto nErr##bail;                          \
   }

#define CATCH(nErr) nErr##bail: if (nErr != 0)

#define IGNORE(x)  ((void) x)


#endif //QDSS_CTRL_UTILS_H
