#ifndef MSMHWIOREG_ESPRESSO_H
#define MSMHWIOREG_ESPRESSO_H

/* This file contains dummy definitions just to get SW to compile with 
   Odyssey msmhwioreg.h file. It will be removed once we can merge the 
   defs into msmhwioreg.h */

/* LTE encoder input buffer, for 8 HARQ banks.
   ENCIB bank 0: 0xB3A00000   (1600x32)
   ENCIB bank 1: 0xB3A02000   (1600x32)
   ...
   ENCIB bank 7: 0xB3A0E000   (1600x32)
*/
#define HWIO_LTE_TX_ENC_IN_MEM_BASE_MIN_ADDR              0xB3A00000
#define HWIO_LTE_TX_ENC_IN_MEM_BASE_MAX_ADDR              0xB3A0FFFC

/* Encoder output buffer. Debug access only.
   ENCOB:        0xB3A20000  (1980x32) 
*/
#define HWIO_LTE_TX_ENC_OUT_MEM_BASE_MIN_ADDR             0xB3A20000
#define HWIO_LTE_TX_ENC_OUT_MEM_BASE_MAX_ADDR             0xB3A21EEC

/* SCME memory, 4 banks. Debug access only. 
   SCME bank 0:  0xB3A40000  (1024x24)
   SCME bank 1:  0xB3A41000  (1024x24)
   SCME bank 2:  0xB3A42000  (1024x24)
   SCME bank 3:  0xB3A43000  (1024x24)
*/
#define HWIO_LTE_TX_SCME_MEM_BASE_MIN_ADDR                0xB3A40000
#define HWIO_LTE_TX_SCME_MEM_BASE_MAX_ADDR                0xB3A43FFC

/* CPIW memory (3 banks). Debug access only. 
   CPIW bank 0:  0xB3A60000  (640x32)
   CPIW bank 1:  0xB3A61000  (640x32)
   CPIW bank 2:  0xB3A62000  (640x32)
*/
#define HWIO_LTE_TX_CPIW_MEM_BASE_MIN_ADDR                0xB3A60000
#define HWIO_LTE_TX_CPIW_MEM_BASE_MAX_ADDR                0xB3A62FFC

#endif



