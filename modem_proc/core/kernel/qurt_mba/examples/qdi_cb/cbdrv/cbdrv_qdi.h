#define CBDRV_REGISTER  (QDI_PRIVATE + 0)
#define CBDRV_NOTIFY    (QDI_PRIVATE + 1)
#define CBDRV_GET_CB    (QDI_PRIVATE + 2)

struct cbdrv_qdi_cbinfo {
   void (*pfn)(void *, int);
   void *arg;
   int value;
};
