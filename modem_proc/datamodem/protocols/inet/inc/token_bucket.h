#ifndef TOKEN_BUCKET_H
#define TOKEN_BUCKET_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                      T O K E N _ B U C K E T . H

DESCRIPTION
  Header containing declarations for Token Bucket API. 

Copyright (c) 2004-2008 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/inet/inc/token_bucket.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
11/19/08    pp     Lint fixes.
07/16/04    vp     Created the file.
===========================================================================*/

#include "comdef.h"
#include "customer.h"



#include "qw.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/

/*---------------------------------------------------------------------------
  Token Bucket structure for rate limiting outgoing packets. For the external
  API we get the values of max_tokens and tokens_per_second to initialize the
  bucket but internally we keep the values in a different format. We keep the 
  values in precision format so that it allows us to keep track of fractional
  tokens generated. Also each time when one needs to get the token from the
  bucket the ARM processor does not have to undergo torture of DIVIDE operator.
  The token bucket contains the following.

  timestamp - Timestamp os the last time we looked at the bucket.

  tokens_precision_m - Number of tokens in the bucket currently left shifted
                       by m. This gives us m precision bits to keep track of
		       fractional tokens.
		       
  max_tokens_precision_m - Max number of tokens that the bucket can hold left
                           shifted by m. This is in accordance with current
			   number of tokens in the bukcet.
			   
  tokens_precision_n_per_ms - Number of tokens generated per millisecond left
                              shifted by n. This gives us precision of n bits
			      to keep track of fractional tokens generated.
			      
  time_to_fill - Time in milliseconds to fill the bucket with max number of 
                 tokens.
  
  m - Number of precision bits for tokens and max_tokens.

  n - Number of precision bits tokens generated per second.

  ********************************* WARNING ********************************
  * PLEASE DO NOT ACCESS THE MEMBERS OF THIS STRUCTURE DIRECTLY.           *
  * PLEASE USE THE ACCESS FUNCTIONS OTHERWISE IT WONT WORK.                *
  * FOR ANY ISSUES PLEASE CONTACT THE AUTHOR.                              *
  **************************************************************************
---------------------------------------------------------------------------*/
typedef struct
{
  qword  timestamp; 
  uint32 tokens_precision_m;   
  uint32 max_tokens_precision_m;
  uint32 tokens_precision_n_per_ms;
  uint32 time_to_fill;
  uint32 m;
  uint32 n;
} token_bucket_type;


/*===========================================================================

                      PUBLIC FUNCTIONS DECLARATIONS

===========================================================================*/
/*===========================================================================
FUNCTION TOKEN_BUCKET_INIT()

DESCRIPTION
  Initialize a token bucket.

DEPENDENCIES
  None

PARAMETERS
  *token_bucket      Token bucket to be initialized. 
  tokens_per_second  Number of tokens to be added per second. 
  max_tokens         Max no. of tokens.
  start_full         Flag to determine whether to initialize bucket as full.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void token_bucket_init
(
  token_bucket_type *token_bucket,
  uint32            tokens_per_second,
  uint32            max_tokens,
  boolean           start_full 
);

/*===========================================================================
FUNCTION TOKEN_BUCKET_GET_TOKENS()

DESCRIPTION
  Get required number of tokens from the passed token bucket if tokens are 
  available.

DEPENDENCIES
  Token bucket should be initialized before calling this function.

PARAMETERS
  *token_bucket - Pointer to the token bucket from which we need the token. 

RETURN VALUE
  FALSE - No token available.
  TRUE  - Number of tokens required are taken out of the bucket and the 
          operation is successful.

SIDE EFFECTS
  Generates tokens and fills up the token bucket according to the time
  elapsed between the last call and current call to this function.
  Sets the timestamp of the token bucket to current time.
===========================================================================*/
boolean token_bucket_get_tokens
(
  token_bucket_type *token_bucket,
  uint32             tokens_req
);

/*===========================================================================
FUNCTION TOKEN_BUCKET_GET_TOKEN()

DESCRIPTION
  Get a token from the passed token bucket if tokens are available.

DEPENDENCIES
  Token bucket should be initialized before calling this function.

PARAMETERS
  *token_bucket - Pointer to the token bucket from which we need the token. 

RETURN VALUE
  0 - No token available.
  1 - One token.

SIDE EFFECTS
  Generates tokens and fills up the token bucket according to the time
  elapsed between the last call and current call to this function.
  Sets the timestamp of the token bucket to current time.
===========================================================================*/
boolean token_bucket_get_token
(
  token_bucket_type *token_bucket
);

/*===========================================================================
FUNCTION TOKEN_BUCKET_SET_MAX_TOKENS()

DESCRIPTION
  Set max_tokens member of token bucket (to increase or decrease after init).

DEPENDENCIES
  None

PARAMETERS
  *token_bucket - Pointer to the token bucket.
  max_tokens    - Max number of tokens the bucket can hold.

RETURN VALUE
  0 - Set failed.
  1 - Set succeeded.

SIDE EFFECTS
  None
===========================================================================*/
boolean token_bucket_set_max_tokens
(
  token_bucket_type *token_bucket,
  uint32             max_tokens
);

/*===========================================================================
FUNCTION TOKEN_BUCKET_SET_TOKENS_PER_SECOND()

DESCRIPTION
  Set tokens_per_second member of token bucket.

DEPENDENCIES
  None

PARAMETERS
  *token_bucket     - Pointer to the token bucket.
  tokens_per_second - Number of tokens added per second.

RETURN VALUE
  0 - Set failed.
  1 - Set succeeded.

SIDE EFFECTS
  None
===========================================================================*/
boolean token_bucket_set_tokens_per_second
(
  token_bucket_type *token_bucket,
  uint32             tokens_per_second
);



#endif /* TOKEN_BUCKET_H */
