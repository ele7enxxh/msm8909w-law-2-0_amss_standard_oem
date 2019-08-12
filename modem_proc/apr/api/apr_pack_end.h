
/*
  Copyright (C) 2009-2010 Qualcomm Technologies Incorporated.
  All rights reserved.
  QUALCOMM Proprietary/GTDR.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/apr/api/apr_pack_end.h#1 $
  $Author: mplcsds1 $
*/

#if defined( __GNUC__ )
  __attribute__((packed))
#elif defined( __arm__ )
#elif defined( _MSC_VER )
  #pragma pack( pop )
#else
  #error Unsupported compiler.
#endif /* __GNUC__ */

