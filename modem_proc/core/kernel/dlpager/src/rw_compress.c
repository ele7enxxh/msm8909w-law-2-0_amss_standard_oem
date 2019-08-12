#include "rw_compress.h"
#include <string.h>
#include <stdint.h>

#include "../compressor/q6zip_constants.py"

/* 2 byte version */
volatile struct
{
  unsigned char major;
  unsigned char minor;
} q6zip_rw_algorithm = {Q6ZIP_RW_ALGORITHM_VERSION >> 8, Q6ZIP_RW_ALGORITHM_VERSION & 0xFF};

#define NUM_ANCHORS 4 // must be a power of 2
#if (NUM_ANCHORS != 4)
#error("NUM_ANCHORS not 4")
#endif
#define NUM_ANCHOR_BITS 2
#define NUM_CODE_BITS 2
#define WORD_SIZE_IN_BITS (sizeof(unsigned int) << 3) // 32

#define GET_BITS(hold,n) (unsigned int)(hold & ((1UL << n) - 1))

#ifdef __hexagon__
#define LIKELY(x) __builtin_expect((x), 1)
#define UNLIKELY(x) __builtin_expect((x), 0)
#else
#define LIKELY(x) (x)
#define UNLIKELY(x) (x)
#endif

#define SKIP_BITS_W_CHECK(compressedInPtr,bitCount,hold,n)      \
  bitCount -= n;                                    \
  hold >>= n;                                                   \
  if (UNLIKELY(bitCount <= WORD_SIZE_IN_BITS))                  \
  {                                                     \
    hold |= (uint64_t)(*(compressedInPtr++)) << bitCount;     \
    bitCount += WORD_SIZE_IN_BITS;                    \
  }

#define pushCompressedBits(bits,numBits,compressedWordStream,numCompressedWords,compressedPartialBits,compressedPartialWord) \
  { \
    unsigned long long int temp = ((unsigned long long int) bits) << compressedPartialBits;                                  \
    compressedPartialBits += numBits;                                                                                        \
    compressedPartialWord = compressedPartialWord|temp;                                                                      \
    if (compressedPartialBits >= WORD_SIZE_IN_BITS)                                                                          \
    {                                                                                                                        \
        compressedWordStream[numCompressedWords++] = compressedPartialWord;                                                  \
        compressedPartialWord = compressedPartialWord >> WORD_SIZE_IN_BITS;                                                  \
        compressedPartialBits -= WORD_SIZE_IN_BITS;                                                                          \
    } \
  }

#define finalizeCompressedBits(compressedWordStream,numCompressedWords,compressedPartialBits,compressedPartialWord) \
  if (compressedPartialBits>0)                                                                                      \
  {                                                                                                                 \
    compressedWordStream[numCompressedWords++] = compressedPartialWord;                                             \
  }

//check for exact or partial match, whichever is first in the anchor list
//code for exact match is 01 and for partial match is 10
#define CHECK_ANCHOR(anchor) \
        anchor_val = anchors[anchor]; \
        if (anchor_val == val) \
        { \
          pushCompressedBits((anchor << NUM_CODE_BITS) + 1,NUM_ANCHOR_BITS + NUM_CODE_BITS,compressed,numCompressedWords,numCompressedPartialBits,compressedPartialWord); \
          continue; \
        } \
        if ((anchor_val & 0xFFFFFC00) == (val & 0xFFFFFC00)) \
        { \
          pushCompressedBits(((val&0x3FF)<<(NUM_ANCHOR_BITS+NUM_CODE_BITS))+((anchor<<NUM_CODE_BITS)+2),NUM_ANCHOR_BITS+NUM_CODE_BITS+10,compressed,numCompressedWords,numCompressedPartialBits,compressedPartialWord); \
          anchors[anchor] = val; \
          continue; \
        } \
        anchor = (anchor + (NUM_ANCHORS - 1)) & (NUM_ANCHORS - 1); \


//check for end and if not, execute the corresponding case
#define JUMP_NEXT_CASE \
{ \
code = GET_BITS(hold,NUM_CODE_BITS); \
void *jump_ptr = jump_table[code]; \
if (out_index < out_len) \
goto *jump_ptr; \
goto RETLAB; \
} \

/*********************************************************************************************************/

//compress a word-aligned buffer uncompressed of length in_len words into a word-aligned buffer compressed
unsigned int deltaCompress(unsigned int *uncompressed,unsigned int *compressed,unsigned int in_len)
{
  unsigned int i,val;
  unsigned int anchors[NUM_ANCHORS],anchor_val;
  unsigned int anchor,anchorIndex = NUM_ANCHORS - 1;
  unsigned int numCompressedPartialBits = 0,numCompressedWords = 0;
  unsigned long long int compressedPartialWord = 0;

  anchors[0] = anchors[1] = anchors[2] = anchors[3] = 0;
  //loop over input
  for (i = 0; i < in_len; i++)
  {
    val = uncompressed[i];
    if (val == 0)
    {
      //code is 00
      pushCompressedBits(0,NUM_CODE_BITS,compressed,numCompressedWords,numCompressedPartialBits,compressedPartialWord);
      //no need to add as anchor as 0 is always a special case
    }
    else
    {
      //non-zero value, check for exact or partial match with anchors
      //traverse the anchors such that most recently seen ones come first
      anchor = anchorIndex;
      CHECK_ANCHOR(anchor);
      CHECK_ANCHOR(anchor);
      CHECK_ANCHOR(anchor);
      CHECK_ANCHOR(anchor);
      //if full or partial match above, control would have hit continue statement
      //no full or partial match, leave the word uncompressed, add it as an anchor
      anchorIndex = (anchorIndex + 1) & (NUM_ANCHORS - 1);
      anchors[anchorIndex] = val;
      //code is 11
      pushCompressedBits(3,NUM_CODE_BITS,compressed,numCompressedWords,numCompressedPartialBits,compressedPartialWord);
      //push the word
      pushCompressedBits(val,WORD_SIZE_IN_BITS,compressed,numCompressedWords,numCompressedPartialBits,compressedPartialWord);
    } //val != 0
  } //for loop
  //take care of the remaining bits
  finalizeCompressedBits(compressed,numCompressedWords,numCompressedPartialBits,compressedPartialWord);
  return (numCompressedWords << 2);
}

//uncompress a word-aligned buffer compressed of length in_len words into a word-aligned buffer uncompressed of of lengh out_len words
unsigned int deltaUncompress(unsigned int *compressed,unsigned int *uncompressed,unsigned int out_len)
{
  unsigned int delta;
  unsigned int code,val,size,anchor;
  unsigned int anchors[NUM_ANCHORS];
  unsigned int anchorIndex = NUM_ANCHORS - 1;
  unsigned int out_index = 0,numAvailBits = 0,bufIndex = 0;
  unsigned long long int hold = 0;
  void* jump_table[] = {
    &&CODE0, //0
    &&CODE1, //1
    &&CODE2, //2
    &&CODE3  //3
  };

  anchors[0] = anchors[1] = anchors[2] = anchors[3] = 0;
  SKIP_BITS_W_CHECK(compressed,numAvailBits,hold,0);
  if (size != 1) SKIP_BITS_W_CHECK(compressed,numAvailBits,hold,0);

  //process the input
      JUMP_NEXT_CASE
CODE0:
      //value was 0
      SKIP_BITS_W_CHECK(compressed,numAvailBits,hold,NUM_CODE_BITS);
      uncompressed[out_index++] = 0;
      JUMP_NEXT_CASE
CODE1:
      //get anchor number which has exact match
      anchor = GET_BITS(hold>>NUM_CODE_BITS,NUM_ANCHOR_BITS);
      SKIP_BITS_W_CHECK(compressed,numAvailBits,hold,NUM_CODE_BITS+NUM_ANCHOR_BITS);
      uncompressed[out_index++] = anchors[anchor];
      JUMP_NEXT_CASE
CODE2:
      //get anchor number which has partial match
      anchor = GET_BITS(hold>>NUM_CODE_BITS,NUM_ANCHOR_BITS);
      //get the delta and construct output word
      delta = GET_BITS(hold>>(NUM_CODE_BITS+NUM_ANCHOR_BITS),10);
      SKIP_BITS_W_CHECK(compressed,numAvailBits,hold,NUM_CODE_BITS+NUM_ANCHOR_BITS+10);
      val = uncompressed[out_index++] = (anchors[anchor] & 0xFFFFFC00) | delta;
      //update existing anchor
      anchors[anchor] = val;
      JUMP_NEXT_CASE
CODE3:
      //no exact or partial match
      SKIP_BITS_W_CHECK(compressed,numAvailBits,hold,NUM_CODE_BITS);
      val = (unsigned int)hold;
      SKIP_BITS_W_CHECK(compressed,numAvailBits,hold,32);
      uncompressed[out_index++] = val;
      //add new anchor
      anchorIndex = (anchorIndex + 1) & (NUM_ANCHORS - 1);
      anchors[anchorIndex] = val;
      JUMP_NEXT_CASE
RETLAB: return (out_index << 2);
}
