#include <stdbool.h>
#include "tlmm_hwio.h"
#include "gpio_debug.h"
#include "msmhwiobase.h"
#include "HALhwio.h"

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
}
