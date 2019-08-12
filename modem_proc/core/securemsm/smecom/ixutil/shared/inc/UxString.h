#ifndef UXSTRING_H
#define UXSTRING_H

/*===========================================================================

                             U X S T R I N G
   
                           H E A D E R  F I L E

FILE:  UxString.h


DESCRIPTION
  A class to manipulate String objects
 

EXTERNALIZED FUNCTIONS : 


  Copyright (c) 2005 by Qualcomm Technologies, Incorporated.  All Rights Reserved.  
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE
                            
  $Header: //components/rel/core.mpss/3.5.c12.3/securemsm/smecom/ixutil/shared/inc/UxString.h#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $ 


when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/11/06   ssm     Enhancements
05/10/06   ssm     Created
===========================================================================*/
#include "comdef.h"

class UxString
{
private:
  char *s;
  int len;

  /* Constructor */
  UxString(int size);

public:
  /* Default Constructor */
  UxString(const char *p="");

  /* copy constructor */
  UxString(const UxString & t);

  /* destructor */
  ~UxString();
	
  UxString & operator=(const UxString &);   

  bool operator==(const UxString &) const;
  
  operator const char * () const { return s; }   
  
  int length() const { return len; }   
  
  friend UxString operator+(const UxString &, const UxString &);
  
  char* c_str() const { return s; }
};

#endif // UXSTRING_H
