#ifndef SECSSL_H
#define SECSSL_H

/*===========================================================================*/

/**
   @file secssl.h
   @brief The Secure Sockets Layer provides authenticated and encrypted 
     on-blocking connectivity to a secure server supporting SSL version 3.0.

 *   SecSSL supports one connection per session. The read/write functions run
 *   partially in the context of the calling task. Due to the additional
 *   overhead of encryption and authentication, secssl_read() and 
 *   secssl_write() impose a larger latency than the regular dss_read()
 *   and dss_write().
 */
/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The SECSSL_API_mainpage.dox file contains all file/group 
      descriptions that are in the output PDF generated using Doxygen and 
      Latex. To edit or update any of the file/group text in the PDF, edit 
      the SECSSL_API_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the "ssl_api" group 
      description in the SECSSL_API_mainpage.dox file. 
===========================================================================*/


#endif

