#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <assert.h>
#include <qurt.h>
#include <qurt_timer.h>

#include <string.h>

void __qdsp6_bsp_init(void)
{
    __asm__ __volatile__ (
        "r11 = r11 \n"
        "r11 = #0x1234\n"
        "jumpr r31  \n"
        :::"r11");
}

int main(int argc, char **argv)
{
  int ret, status;

  printf("qurt pid is %d\n", qurt_getpid());

  cbdrv_qdi_init();

  qurt_spawn("app1.pbn");
  qurt_spawn("app2.pbn");

    ret = qurt_wait(&status);
    printf("guest OS detected process %d exits with status =%x\n",ret, status);
    
    ret = qurt_wait(&status);
    printf("guest OS detected process %d exits with status =%x\n",ret, status);

    
  printf("qurt main exit.\n");

  return 0;
}

