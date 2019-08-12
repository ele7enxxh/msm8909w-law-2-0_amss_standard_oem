/*

  Copyright © 2013 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  QUALCOMM Confidential and Proprietary
  
*/

#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>  // for format specifier for uint64_t
#include <string.h>
#include <assert.h>
#include "qrlbn_arith.h"
#include "qrlbn_ecc_types.h"
#include "qrlbn_ecc_ops.h"
#include "qrlbn_utils.h"
// #include "qrlbn_ecc_internal.h"

static int
printbn(qrlbn_ecc_bigval_t *a, const qrlbn_modulus_data_t *mdp, char *line_ender, FILE *f)
{
   int i;
   
   fprintf(f, "{{");
   for (i = 0; i < mdp->num_words; ++i) {
      fprintf(f, "0x%x", a->data[i]);
      if (i < mdp->num_words - 1) {
	 fprintf(f, ", ");
      }
   }
   fprintf(f, "}}%s", line_ender);

   return (0);
}

static int
print_modulus_data(qrlbn_ecc_modulus_data_t *emdp, char *line_ender, FILE *f)
{
   // Take off flags that indicate pointers
   // PRIx64 expands to something like llx, but approprate for the platform.
   fprintf(f, "{/* flags num_words modulus_bits 3_data_offsets m_inv quotient_shift m_recip */\n{0x%x, %d, %d, {%d}, {%d}, {%d}, 0x%x, %d, 0x%" PRIx64 "},\n", 
	   emdp->md.flags & ~0x7, emdp->md.num_words, emdp->md.modulus_bits,
	   offsetof(qrlbn_ecc_modulus_data_t, modulus_storage),
	   offsetof(qrlbn_ecc_modulus_data_t, montgomery_R_storage),
	   offsetof(qrlbn_ecc_modulus_data_t, montgomery_R2_storage),
	   emdp->md.m_inv32,
	   emdp->md.quo_num_shift,
	   emdp->md.m_recip);
   printbn(&emdp->modulus_storage, &emdp->md, ",\n", f);
   printbn(&emdp->montgomery_R_storage, &emdp->md, ",\n", f);
   printbn(&emdp->montgomery_R2_storage, &emdp->md, "}", f);
   fprintf(f, "%s", line_ender);

   return(0);
}

int
qrlbn_print_ecc_domain(qrlbn_ecc_domain_t *dp, char *curve_name, FILE *f)
{
   fprintf(f, "{ /* domain initializer for %s */\n", curve_name); 
   fprintf(f, "/* version */ %d,\n", dp->version);
   fprintf(f, "/* modulus */\n");
   print_modulus_data(&dp->modulus, ",\n", f);
   fprintf(f, "/* base point order*/\n");
   print_modulus_data(&dp->base_point_order, ",\n", f);
   fprintf(f, "/* a (montgomery) */\n");
   printbn(&dp->a, &dp->modulus.md, ",\n", f);
   fprintf(f, "/* b (montgomery) */\n");
   printbn(&dp->b, &dp->modulus.md, ",\n", f);
   fprintf(f, "{ /* affine base point */\n");
   printbn(&dp->affine_base_point.x, &dp->modulus.md, ",\n", f);
   printbn(&dp->affine_base_point.y, &dp->modulus.md, ",\n", f);
   fprintf(f, "0x%x},\n", dp->affine_base_point.flags);
   fprintf(f, "{ /* jacobian base point (montgomery) */\n");
   printbn(&dp->base_point.X, &dp->modulus.md, ",\n", f);
   printbn(&dp->base_point.Y, &dp->modulus.md, ",\n", f);
   printbn(&dp->base_point.Z, &dp->modulus.md, ",\n", f);
   fprintf(f, "0x%x},\n", dp->base_point.flags);
   fprintf(f, "/* cofactor */\n");
   fprintf(f, "%llu}\n", dp->cofactor);

   return(0);
}
