#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "q6zip_uncompress.h"
//#include "hexagon_sim_timer.h"
void q6zip_end_of_stream(void);

int data_size;
int out_size;
char* out_buf;
FILE* fp_data;
FILE* fp_dict;
FILE* fp_expected;
FILE* fp_out;
uint32_t* expected_buf;

typedef enum
{
  SUCCESS = 0,
  CMD_PARAM_USAGE_ERR = 1,
  INPUT_FOPEN_ERR = 2,
  OUTPUT_FOPEN_ERR = 3,
  EXPECTED_OUTPUT_FOPEN_ERR = 4,
  DATA_BUF_MALLOC_ERR = 5,
  DICT_BUF_MALLOC_ERR = 6,
  EXPECTED_OUT_BUF_MALLOC_ERR = 7,
  OUT_BUF_MALLOC_ERR = 8,
  OUTPUT_WRITE_ERR = 9
}err_info_t;

err_info_t dbg_last_err = SUCCESS;

int main(int argc, char* argv[]) {

  if (argc < 4)
  {
    fprintf (stderr,"Usage: %s <to_decompress> <dictionary> <output_decompressed> [<expected_out>] "
       "[<subset-start>] [<subset-size>]\n",
       argv[0] );
	dbg_last_err = CMD_PARAM_USAGE_ERR;
	return dbg_last_err;
    //exit(-2);
  }

  fp_data = fopen(argv[1], "rb");
  if(!fp_data)
  {
    dbg_last_err = INPUT_FOPEN_ERR;
	return dbg_last_err;
  }  
  //assert(fp_data);
 
  fp_dict = fopen(argv[2], "rb");
  if(!fp_dict)
  {
    dbg_last_err = INPUT_FOPEN_ERR;
	return dbg_last_err;
  }  
  //assert(fp_dict);
  
  fp_out = fopen(argv[3], "wb");
  if(!fp_out)
  {
    dbg_last_err = OUTPUT_FOPEN_ERR;
	return dbg_last_err;  
  }
  //assert(fp_out);

  fp_expected = 0;

  if (argc > 4)
  {
    printf("uncompressed %s\n", argv[4]);
    fp_expected = fopen(argv[4], "rb");
    //assert(fp_expected);   
    dbg_last_err = EXPECTED_OUTPUT_FOPEN_ERR;
	return dbg_last_err;  	
  }
 
  fseek(fp_data, 0L, SEEK_END);
  data_size = ftell(fp_data);
  fseek(fp_data, 0L, SEEK_SET);
  printf("compressed size %d\n", data_size);

  fseek(fp_dict, 0L, SEEK_END);
  int dict_size = ftell(fp_dict);
  fseek(fp_dict, 0L, SEEK_SET);
  printf("dictionary size %d\n", dict_size);

  int expected_size = 0;
  int subset_start = 0;
  if (fp_expected)
  {
    if (argc > 6)
    {
      expected_size = strtol(argv[6], 0, 16);
    }
    else
    {
      fseek(fp_expected, 0L, SEEK_END);
      expected_size = ftell(fp_expected);
    }
    fseek(fp_expected, 0L, SEEK_SET);
    if (argc > 5)
    {
      subset_start = strtol(argv[5], 0, 16);
      if (subset_start > 4) subset_start -= 4;
      printf("subset-start 0x%08X\n", subset_start);
    }
    fseek(fp_expected, subset_start, SEEK_SET);
    printf("expected size %d\n", expected_size);
  }

  char* data_buf = (char*)malloc (data_size);
  if(!data_buf)
  {
    dbg_last_err = DATA_BUF_MALLOC_ERR;
	return dbg_last_err;
	//assert (data_buf);
  }

#ifndef SWAP_ENDIAN
  int data_buf_size = fread(data_buf, 1, data_size , fp_data);
#else
  int data_buf_size = 0;
  int i, j;
  for (i = 0; i < data_size; i += 4)
  {
    data_buf_size += fread(data_buf + i + 3, 1, 1 , fp_data);
    data_buf_size += fread(data_buf + i + 2, 1, 1 , fp_data);
    data_buf_size += fread(data_buf + i + 1, 1, 1 , fp_data);
    data_buf_size += fread(data_buf + i + 0, 1, 1 , fp_data);
//    printf("%02X %02X %02X %02X ", *(data_buf + i + 0), *(data_buf + i + 1), *(data_buf + i + 2), *(data_buf + i + 3));
  }
  printf("\n");
#endif
  
  char* dict_buf = (char*)malloc (dict_size);
  if(!dict_buf)
  {
    dbg_last_err = DICT_BUF_MALLOC_ERR;
	return dbg_last_err;  
    //assert (dict_buf);
  }
#ifndef SWAP_ENDIAN
  int dict_buf_size = fread(dict_buf, 1, dict_size , fp_dict);
#else
  int dict_buf_size = 0;
  int i;
  for (i = 0; i < dict_size; i += 4)
  {
    dict_buf_size += fread(dict_buf + i + 3, 1, 1 , fp_dict);
    dict_buf_size += fread(dict_buf + i + 2, 1, 1 , fp_dict);
    dict_buf_size += fread(dict_buf + i + 1, 1, 1 , fp_dict);
    dict_buf_size += fread(dict_buf + i + 0, 1, 1 , fp_dict);
//    printf("%02X %02X %02X %02X ", *(dict_buf + i + 0), *(dict_buf + i + 1), *(dict_buf + i + 2), *(dict_buf + i + 3));
  }
  printf("\n");
#endif

  out_size = 4096;//default value.

  if (fp_expected)
  {
    expected_buf = (uint32_t*)malloc (expected_size);
	if(!expected_buf)
    {
      dbg_last_err = EXPECTED_OUT_BUF_MALLOC_ERR;
	  return dbg_last_err; 
      //assert (expected_buf);
	}
    int expected_buf_size = fread(expected_buf, 1, expected_size, fp_expected);
    /* int i; */
    /* for (i = 0; i < expected_size/4; ++i) */
    /*  printf("0x%08X \n", expected_buf[i]); */
    out_size = expected_size;
  }


  printf("decompressed size %d\n" , out_size);
//  out_buf = (char*)(((unsigned int)((char*)malloc (out_size))));
  out_buf = (char*)(((unsigned int)((char*)malloc (out_size + 0x1000)) + 0x1000) & ~0xFFF);
//  out_buf = (char*)(((unsigned long)((char*)malloc (out_size + 0x1000)) + 0x1000) & ~0xFFF);
  if(!out_buf)
  {
    dbg_last_err = OUT_BUF_MALLOC_ERR;
	return dbg_last_err; 
    //assert (out_buf);
  }
  int total_out = 0;
  int total_count = 0;


#if 0 
 unsigned long long int start_time,end_time;

  printf("hello\n");
  {
     int i,sum;
     for (i=0;i<dict_size;i++) sum+=*(dict_buf+i);
     printf("hello=%d\n",sum);
  }

  start_time=hexagon_sim_read_pcycles();
#endif
  q6zip_state_t state;
  state.hold_low = *((uint32_t*)data_buf);
  state.hold_high = *((unsigned*)(data_buf + 4));
  state.bits_in_hold = 64;
  state.last_sequencial_offset = -1; /* none, just last word */

  q6zip_uncompress(out_buf, data_buf + 8, dict_buf, &state);
#if 0
  end_time=hexagon_sim_read_pcycles();
  printf("pCycles=%d\n",(int)(end_time-start_time));
  printf("hello\n");
  printf("hello\n");
#endif
  q6zip_end_of_stream();
  
  return dbg_last_err;
  //return 0;
}

void q6zip_end_of_stream(void)
{
  //output to decompressed.bin file.
  unsigned long output_size = 0;
  unsigned long output_size2 = 0;
  //printf("Total size of output file is: %lu\n", out_size);
  output_size = fwrite( out_buf, sizeof(out_buf[0]), (out_size/sizeof(out_buf[0])), fp_out );
  if(output_size != out_size/sizeof(out_buf[0]))
  {
	dbg_last_err = OUTPUT_WRITE_ERR;
	fclose( fp_out );
	return;
	//assert(output_size == out_size/sizeof(out_buf[0]));
  }

  fclose( fp_out );

#if 0
  printf("in size %d out size %d ratio %2.3f\n", data_size, out_size, (float)data_size / out_size);
  if (fp_expected)
  {
    int r = memcmp(out_buf, expected_buf, out_size);
    if (r)
    {
      printf("memcmp %d out 0x%08X\n", r, (unsigned int)out_buf);
      int i;

      unsigned int* p = (unsigned int*)out_buf;
      unsigned int* p2 = (unsigned int*)expected_buf;
      for (i = 0; i < out_size/4; ++i)
      {
	      if (p[i] != p2[i]) printf("***");
	      {
	        printf("%8u %08X %08X\n", i, p[i], p2[i]);
	      }
      }  
    }
  }
#endif /*0*/

  printf("RO decompression completed.\n");

  fclose(fp_data);
  fclose(fp_dict);
  if (fp_expected)
  {
    fclose(fp_expected);
  }
  exit(0);
}
