#include <stdbool.h>
#include "tlmm_hwio.h"
#include "gpio_debug.h"
#include "msmhwiobase.h"
#include "HALhwio.h"

#define GPIO_TEST_ID    55                                     //only GPIO pin RPM has access to on 8974

static unsigned          gpio_debug_application_bitindex  = 0; //set which transitions are enabled
static unsigned          gpio_debug_transition_bitindex   = 0; //set both sleep and active transtions are viewable
int                      bank                             = GPIO_TEST_ID/32; // math for the bank

/* Toggles GPIO on, 2v *Note, 2v is default level
 * 
 * gpio_number: The gpio line being accessed 
 *  
 * Note: These functions are only valid for GPIO pins 
 * 32-63 because the HWIO_TLMM register is for these pins only.  
 *  
 */
void gpio_toggle_high(void)
{
    HWIO_TLMM_GPIO_CFGn_OUTI(GPIO_TEST_ID,0x303); 
    switch(bank)
    {
        case 0:
                    HWIO_TLMM_GPIO_OUT_SET_0_OUT( 1 << (GPIO_TEST_ID%32) );
                    break;
        case 1:
                    HWIO_TLMM_GPIO_OUT_SET_1_OUT( 1 << (GPIO_TEST_ID%32) );
                    break;
        case 2:
                    HWIO_TLMM_GPIO_OUT_SET_2_OUT( 1 << (GPIO_TEST_ID%32) );
                    break;
        case 3:
                    HWIO_TLMM_GPIO_OUT_SET_3_OUT( 1 << (GPIO_TEST_ID%32) );
                    break;
        default:
                    break;
    }
}

/* Toggles GPIO off, 0v *Note, 2v is default level
 * 
 * gpio_number: The gpio line being accessed 
 *  
 * Note: These functions are only valid for GPIO pins 
 * 32-63 because the HWIO_TLMM register is for these pins only.  
 *  
 */
void gpio_toggle_low(void)
{
    HWIO_TLMM_GPIO_CFGn_OUTI(GPIO_TEST_ID,0x303); 
    switch(bank)
    {
        case 0:
                    HWIO_TLMM_GPIO_OUT_CLR_0_OUT( 1 << (GPIO_TEST_ID%32) );
                    break;
        case 1:
                    HWIO_TLMM_GPIO_OUT_CLR_1_OUT( 1 << (GPIO_TEST_ID%32) );
                    break;
        case 2:
                    HWIO_TLMM_GPIO_OUT_CLR_2_OUT( 1 << (GPIO_TEST_ID%32) );
                    break;
        case 3:
                    HWIO_TLMM_GPIO_OUT_CLR_3_OUT( 1 << (GPIO_TEST_ID%32) );
                    break;
        default:
                    break;
    }
}

/*  Used for timing profiling
 * 
 *  gpio_application: index of the application calling the gpio_test_high(), when & with gpio_debug_application_bitindex and
 *  creates a non-zero if valid.
 *  
 *  gpio_transition: index of the transition calling the gpio_test_high(), when & with gpio_debug_transition_bitindex and
 *  creates a non-zero if valid.
 *  
 */
void gpio_test_high( gpio_debug_application gpio_application, gpio_debug_transition gpio_transition )
{
    //Both bitindex must be set to non-zero for the gpio_test to toggle
    if (((1 << gpio_application) & gpio_debug_application_bitindex) && ((1 << gpio_transition) & gpio_debug_transition_bitindex)) 
    {
        gpio_toggle_high();
    }
}

/*  Used for timing profiling
 * 
 *  gpio_application: index of the application calling the gpio_test_high(), when & with gpio_debug_application_bitindex and
 *  creates a non-zero if valid.
 *  
 *  gpio_transition: index of the transition calling the gpio_test_high(), when & with gpio_debug_transition_bitindex and
 *  creates a non-zero if valid.
 *  
 */
void gpio_test_low( gpio_debug_application gpio_application, gpio_debug_transition gpio_transition )
{
    //Both bitindex must be set to non-zero for the gpio_test to toggle
    if (((1 << gpio_application) & gpio_debug_application_bitindex) && ((1 << gpio_transition) & gpio_debug_transition_bitindex)) 
    {
        gpio_toggle_low();
    }
}
