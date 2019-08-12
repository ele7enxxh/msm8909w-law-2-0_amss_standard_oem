void cbdrv_init(void);
void cbdrv_register_callback(void (*pfn)(void *, int),
                             void *arg);
void cbdrv_notify_callbacks(int value);
