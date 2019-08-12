#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <qurt.h>

// Needed for callig Dynamic Linker functions like dlinit, dlopen, dlsym etc.
#include <dlfcn.h>

#define MAX_THREADS 6 

int ceiling_prio = (255-200);
static qurt_sem_t to_start; 
static int t[MAX_THREADS];
int original_prio[MAX_THREADS];

void __hexagon_bsp_init(void)
{
    __asm__ __volatile__ (
        "r11 = r11 \n"
        "r11 = #0x1234\n"
        "jumpr r31  \n"
        :::"r11");
}


static int execute_so_func(unsigned char* handle, char *func_name, int param)
{
    int (*genfunc)(int);
    if(!handle)
    {
        printf("BAD Handle Passed in %s\n", __func__);
        return -1;
    }
    if(!func_name)
    {
        printf("BAD Func Name Passed in %s\n", __func__);
        return -1;   
    }
    printf ("Looking up function %s\n", func_name);
    genfunc = dlsym(handle, func_name);
    if (!genfunc)
    {
        printf("dlsym failed for function %s\n", func_name);
        return -1;
    }
    printf ("Calling function %s at address 0x%p\n", func_name, genfunc);
    return genfunc(param);
}

void run_mod_so_test(char *so_name, int load_type, int module_num)
{
    unsigned char *handle;
    char func_name[64];
    int (*genfunc)(int);
    int ret_val, dlopen_reached = 0;

    if (RTLD_NOW == load_type)
        printf("Running Test for SO %s with bind type RTLD_NOW\n", so_name);
    else
        printf("Running Test for SO %s with bind type RTLD_LAZY\n", so_name);

    do
    {
        printf("Calling dlopen");
        handle = dlopen (so_name, load_type);
        printf("Returned dlopen: 0x%x", handle);
        if (!handle)
        {
            printf("dlopen() Failed!!!\n");
            break;
        }
        dlopen_reached = 1;

        /* Call Init Func */
        snprintf(func_name, sizeof(func_name), "%s%d", "init_module_" , module_num);
        ret_val = execute_so_func(handle, func_name, 1);
        assert(1 == ret_val);
        printf("\n");
        /* Call Start Func */
        snprintf(func_name, sizeof(func_name), "%s%d", "start_module_" , module_num);
        ret_val = execute_so_func(handle, func_name, 2);
        assert(2 == ret_val);
        printf("\n");
        /* Call Stop Func */
        snprintf(func_name, sizeof(func_name), "%s%d", "stop_module_" , module_num);
        ret_val = execute_so_func(handle, func_name, 3);
        assert(3 == ret_val);
        printf("\n");
    } while(0);
    
    if(dlopen_reached)
    {
        printf ("Done Calling dlclose\n");
        dlclose(handle);    
    }
}

extern int dlinit(int, char **);
int main(int argc, char **argv)
{
   
   unsigned char *handle;
   char func_name[64];
   int (*genfunc)(int);
   int ret_val, dlopen_reached = 0;
   
   printf( "\n**main()**\n");
   /* Mandatory DL Init */
   printf("Calling DL Init: %x \n", dlinit);
   ret_val = dlinit(0,0); // New dlinit prototype: dlinit(int, char**)
   if (1 != ret_val)
   {
	 printf("dlinit() Failed!!! Err Code:%d \n", ret_val);
	 return 1;
   }


   /* Start the Tests */
   printf("############################################################\n");
   printf("###########   Module I Tests   #############################\n");
   printf("################   Mod I Bind Now   ########################\n");
   run_mod_so_test("module_1.so", RTLD_NOW, 1); //2
   printf("################   Mod I Bind Lazy  ########################\n");
   run_mod_so_test("module_1.so", RTLD_LAZY, 1); //1
   printf("############################################################\n");
   printf("\n\n");

   printf("############################################################\n");
   printf("###########   Module II Tests   ############################\n");
   printf("################   Mod II Bind Now   #######################\n");
   run_mod_so_test("module_2.so", RTLD_NOW, 2); //2
   printf("################   Mod II Bind Lazy  #######################\n");
   run_mod_so_test("module_2.so", RTLD_LAZY, 2); //1
   printf("############################################################\n");
   printf("\n\n");

   /* Exiting Main */
   printf( "\n**all done**\n");
   return 0;
}

