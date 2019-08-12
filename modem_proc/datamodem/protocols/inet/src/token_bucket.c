/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                      T O K E N _ B U C K E T . C

GENERAL DESCRIPTION
   Token bucket API for rate-limiting outgoing traffic.

EXTERNALIZED FUNCTIONS
  token_bucket_init()
    Initialize a token bucket.

  token_bucket_get_token()
    Get a token from the passed token bucket if tokens are available.

  token_bucket_set_max_tokens()
    Set max_tokens member of token bucket (to increase/decrease after init).
    
  token_bucket_set_tokens_per_second()
    Set ms_per_token member of token bucket (to increase/decrease after init).

Copyright (c) 2004-2010 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

 $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/inet/src/token_bucket.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
06/26/10    pp     CMI: clk.h => time_svc.h.
06/09/09    mga    Merged Compiler warnings fixes
04/17/09    pp     assert.h=>amssassert.h
11/29/08    pp     Replaced LOG_MSG_FATAL_ERROR w/ ASSERT.
11/19/08    pp     Lint fixes.
07/11/06    vp     Fixed an if check in token_bucket_get_tokens().
07/16/04    vp     Created the module.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"
#include "msg.h"
#include "amssassert.h"
#include "token_bucket.h"
#include "time_svc.h"
#include "ds_Utils_DebugMsg.h"

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                      LOCAL FUNCTION DECLARATIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
void token_bucket_set_n
(
  token_bucket_type *token_bucket,
  uint32             tokens_per_second
);

void token_bucket_set_m
(
  token_bucket_type *token_bucket,
  uint32             max_tokens
);
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                      PUBLIC FUNCTION DEFINITIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

/*===========================================================================
  ********************************* WARNING ********************************
  * PLEASE DO NOT CHANGE ANYTHING IN THE ACCESS FUNCTIONS BELOW.           *
  * FOR ANY ISSUES PLEASE CONTACT THE AUTHOR.                              *
  **************************************************************************
===========================================================================*/

/*===========================================================================
FUNCTION TOKEN_BUCKET_INIT()

DESCRIPTION
  Initialize a token bucket. For the external interface the function takes
  tokens_per_second and max_tokens as parameters. Internally, in the token
  bucket, the tokens per second are stored as tokens_precision_n_per_ms. There
  is a precision parameter n kept by the token bucket internally (whose value 
  depends on the magnitude of tokens_per_second required). tokens_per_second
  is shifted left by n bits and divided by 1000 to give 
  tokens_precision_n_per_ms. This enables us to have n number of precision 
  bits (fractional value) and it saves us the operation of divide every time
  a token is required.
  Similarly there is a precision parameter m kept internally by the token 
  bucket whose value depends upon the magnitude of max_tokens required. We
  store the max_tokens and current number of tokens in the bucket left shifted
  by m which gives us m precision bits to keep track of fractional tokens
  generated. This prevents any short changing of tokens that might happen in 
  case of whole numbers as m precision bits are added.

PARAMETERS
  *token_bucket      Token bucket to be initialized. 
  tokens_per_second  Number of tokens to be added per second. 
  max_tokens         Max no. of tokens.
  start_full         Flag to determine whether to initialize bucket as full.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void token_bucket_init
(
  token_bucket_type *token_bucket,
  uint32             tokens_per_second,
  uint32             max_tokens,
  boolean            start_full
)
{
  /*-------------------------------------------------------------------------
    Sanity check.
  -------------------------------------------------------------------------*/
  if((token_bucket == NULL) || (tokens_per_second == 0) || (max_tokens == 0))
  {
    DATA_ERR_FATAL("Failed to initialize token bucket, invalid parameter(s)");
    return;
  }

  memset(token_bucket,0,sizeof(token_bucket_type));

  /*-------------------------------------------------------------------------
    The following order of functions and assignments is important as it
    ensures that the token_bucket is initialized correctly.
    It is because the value of precision parameter n needs to be greater than
    precision parameter m. n is calculated in function set_tokens_per_second()
    m is calculated after n in set_max_tokens().
  -------------------------------------------------------------------------*/
  (void)token_bucket_set_tokens_per_second(token_bucket, tokens_per_second);

  (void)token_bucket_set_max_tokens(token_bucket, max_tokens);

  if(start_full)
  {
    token_bucket->tokens_precision_m = token_bucket->max_tokens_precision_m;
  }

  /*-------------------------------------------------------------------------
    Set the token bucket timestamp to current time.
  -------------------------------------------------------------------------*/
  time_get_ms(token_bucket->timestamp);

  return;
} /* token_bucket_init() */

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
)
{
  qword  current_time;
  qword  interval;
  uint32 new_tokens_precision_m;
  uint32 tok_precision_m;

  if(token_bucket == NULL || tokens_req == 0)
  {
    DATA_ERR_FATAL("Null token bucket passed to token_bucket_get_token()");
    return FALSE;
  }

  if(tokens_req > (token_bucket->max_tokens_precision_m >> token_bucket->m))
  {
    DATA_ERR_FATAL("Failed to initialize token bucket, invalid parameter(s)");
    return FALSE;
  }
  time_get_ms(current_time);

  qw_sub(interval, current_time, token_bucket->timestamp);

  /*-------------------------------------------------------------------------
    Convert the required number of tokens into precision m.
  -------------------------------------------------------------------------*/
  tok_precision_m = (tokens_req << token_bucket->m);

  /*-------------------------------------------------------------------------
    If the interval is more than the time required to fill up the bucket
    we can skip over the rest of calculations and fill up the bucket upto max.
  -------------------------------------------------------------------------*/
  if(qw_lo(interval) > token_bucket->time_to_fill)
  {
    /*-----------------------------------------------------------------------
      new_tokens are equal to max tokens the bucket can hold.
    -----------------------------------------------------------------------*/
    new_tokens_precision_m = token_bucket->max_tokens_precision_m;
  }
  else
  {
    /*-----------------------------------------------------------------------
      Since we store tokens as tokens_precision_m (left shifted by m bits)
      we perform all the calculations in precision m. Note that since we 
      stored token_per_second as tokens_precision_n_per_ms we directly get
      new tokens generated in precision n if we multiply interval(in 
      milliseconds) with tokens_precision_n_per_ms. We then shift that value
      by n-m to get new_tokens_precision_m. (We always make sure that n>=m)
    -----------------------------------------------------------------------*/
    new_tokens_precision_m = 
              (qw_lo(interval) * token_bucket->tokens_precision_n_per_ms) >> 
              (token_bucket->n - token_bucket->m);
  }

  token_bucket->tokens_precision_m += new_tokens_precision_m;

  token_bucket->tokens_precision_m = MIN(token_bucket->tokens_precision_m, 
                                     token_bucket->max_tokens_precision_m);

  if(new_tokens_precision_m > 0)
  {
    /*-----------------------------------------------------------------------
      Set the timestamp to current time if we have some new_tokens_precision_m
    -----------------------------------------------------------------------*/
    qw_equ(token_bucket->timestamp, current_time);
  }

  /*-------------------------------------------------------------------------
    If we have at least 1 token to return in the token_bucket we subtract
    1 from the number of tokens and return one token.All the comparisons and 
    calculations are in precision m.
  -------------------------------------------------------------------------*/
  if(token_bucket->tokens_precision_m >= tok_precision_m)
  {
    token_bucket->tokens_precision_m -= tok_precision_m;
    return TRUE;
  } 

  return FALSE;
} /* token_bucket_get_tokens() */

/*===========================================================================
FUNCTION TOKEN_BUCKET_GET_TOKEN()

DESCRIPTION
  Get a token from the passed token bucket if tokens are available.

DEPENDENCIES
  Token bucket should be initialized before calling this function.

PARAMETERS
  *token_bucket - Pointer to the token bucket from which we need the token. 

RETURN VALUE
  FALSE - No token available.
  TRUE  - One token.

SIDE EFFECTS
  Generates tokens and fills up the token bucket according to the time
  elapsed between the last call and current call to this function.
  Sets the timestamp of the token bucket to current time.
===========================================================================*/
boolean token_bucket_get_token
(
  token_bucket_type *token_bucket
)
{
  return token_bucket_get_tokens(token_bucket, 1);
} /* token_bucket_get_token() */

/*===========================================================================
FUNCTION TOKEN_BUCKET_SET_N()

DESCRIPTION
  Set the precision parameter n for the bucket.

DEPENDENCIES
  None.

PARAMETERS
  *token_bucket     - Pointer to the token bucket. 
  tokens_per_second - Rate of generation of tokens

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void token_bucket_set_n
(
  token_bucket_type *token_bucket,
  uint32             tokens_per_second
)
{
  /*-------------------------------------------------------------------------
    The value of n = MAX((16 - ln(tokens_per_second)), 0)
    where ln is log to the base 2.
  -------------------------------------------------------------------------*/
  token_bucket->n = 17;
  while(token_bucket->n && tokens_per_second)
  {
    token_bucket->n--;
    tokens_per_second >>= 1;
  }
} /* token_bucket_set_n() */
/*===========================================================================
FUNCTION TOKEN_BUCKET_SET_M()

DESCRIPTION
  Set the precision parameter m for the bucket.

DEPENDENCIES
  None.

PARAMETERS
  *token_bucket - Pointer to the token bucket. 
  max_tokens    - Max number of tokens allowed for the bucket.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void token_bucket_set_m
(
  token_bucket_type *token_bucket,
  uint32             max_tokens
)
{
  /*-------------------------------------------------------------------------
    The value of m = MIN((16 - ln(max_tokens)), n)
    where ln is log to the base 2.
    Note that m cannot be more than n.
  -------------------------------------------------------------------------*/
  token_bucket->m = 17;
  while(token_bucket->m && max_tokens)
  {
    token_bucket->m--;
    max_tokens >>= 1;
  }
  if(token_bucket->m > token_bucket->n)
  {
    token_bucket->m = token_bucket->n;
  }
}
/*===========================================================================
FUNCTION TOKEN_BUCKET_SET_MAX_TOKENS()

DESCRIPTION
  Set max_tokens in token bucket.

DEPENDENCIES
  None

PARAMETERS
  *token_bucket - Pointer to the token bucket.
  max_tokens    - Max number of tokens the bucket can hold.

RETURN VALUE
  FALSE - Set failed.
  TRUE  - Set succeeded.

SIDE EFFECTS
  None
===========================================================================*/
boolean token_bucket_set_max_tokens
(
  token_bucket_type *token_bucket,
  uint32             max_tokens
)
{
  uint32 old_m;

  if(token_bucket == NULL || max_tokens == 0)
  {
    DATA_ERR_FATAL("Invalid parameter(s) to token_bucket_set_max_tokens()");
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    We start by finding the value of precision variable m.
  -------------------------------------------------------------------------*/
  old_m = token_bucket->m;

  token_bucket_set_m(token_bucket, max_tokens);

  token_bucket->max_tokens_precision_m = max_tokens << token_bucket->m;
  /*-------------------------------------------------------------------------
    If m changes then it changes the value of tokens_precision_m.
  -------------------------------------------------------------------------*/
  if((token_bucket->tokens_precision_m != 0) && (old_m != token_bucket->m))
  {
    if(old_m > token_bucket->m)
    {
      token_bucket->tokens_precision_m >>= (old_m - token_bucket->m);
    }
    else
    {
      token_bucket->tokens_precision_m <<= (token_bucket->m - old_m);
    }
  }

  /*-------------------------------------------------------------------------
    Changing the maximum number of tokens also changes the time_to_fill
    for the token_bucket. time_to_fill needs to change accordingly.
  -------------------------------------------------------------------------*/
  if(token_bucket->tokens_precision_n_per_ms != 0)
  {
    token_bucket->time_to_fill = 
    ((max_tokens << token_bucket->m)/token_bucket->tokens_precision_n_per_ms)
    << (token_bucket->n - token_bucket->m);
  }

  return TRUE;
} /* token_bucket_set_max_tokens() */

/*===========================================================================
FUNCTION TOKEN_BUCKET_SET_TOKENS_PER_SECOND()

DESCRIPTION
  Set the rate of generation of tokens in token bucket (to increase/decrease 
  after init).

DEPENDENCIES
  None

PARAMETERS
  *token_bucket     - Pointer to the token bucket.
  tokens_per_second - Number of tokens added per second.

RETURN VALUE
  FALSE - Set failed.
  TRUE  - Set succeeded.

SIDE EFFECTS
  None
===========================================================================*/
boolean token_bucket_set_tokens_per_second
(
  token_bucket_type *token_bucket,
  uint32             tokens_per_second
)
{
  uint32 old_m;

  if(token_bucket == NULL || tokens_per_second == 0)
  {
    DATA_ERR_FATAL("Invalid parameter(s) to token_bucket_set_ms_per_token()");
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    We start by finding the value of precision variable n.
    We also see whether there was any change in m and adjust the values in
    token bucket accordingly.
  -------------------------------------------------------------------------*/
  token_bucket_set_n(token_bucket,tokens_per_second);
  if(token_bucket->max_tokens_precision_m != 0)
  {
    old_m = token_bucket->m;
    token_bucket_set_m(token_bucket, 
                       (token_bucket->max_tokens_precision_m >> 
                        token_bucket->m)
                      ); 

    /*-----------------------------------------------------------------------
      Now if the new value of m comes out to be different than it was before
      we need to change (left_shift / right_shift) tokens_precision_m and
      max_tokens_precision_m accordingly.
    -----------------------------------------------------------------------*/
    if(old_m != token_bucket->m )
    {
      if(old_m > token_bucket->m)
      {
        token_bucket->tokens_precision_m >>= (old_m - token_bucket->m);
        token_bucket->max_tokens_precision_m >>= (old_m - token_bucket->m);
      }
      else
      {
        token_bucket->tokens_precision_m <<= (token_bucket->m - old_m);
        token_bucket->max_tokens_precision_m <<= (token_bucket->m - old_m);
      }
    }
  }

  /*-------------------------------------------------------------------------
    Now that we have n we shift tokens_per_second by n and divide by 1000 to
    get tokens_precision_n_per_ms (per millisecond).
  -------------------------------------------------------------------------*/
  token_bucket->tokens_precision_n_per_ms = 
                               (tokens_per_second << token_bucket->n) / 1000;

  /*-------------------------------------------------------------------------
    Changing the rate of generation of tokens also changes the time_to_fill
    for the token_bucket.  time_to_fill needs to change accordingly.
  -------------------------------------------------------------------------*/
  token_bucket->time_to_fill = 
     ((token_bucket->max_tokens_precision_m >> token_bucket->m) * 1000) / 
     tokens_per_second;
  return TRUE;
} /* token_bucket_set_ms_per_token() */

