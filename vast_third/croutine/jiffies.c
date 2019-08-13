
#include <types.h>
#include <vast_third/croutine/jiffies.h>

#ifdef __CC_ARM
# pragma diag_suppress 68
#endif

volatile time_t jiffies = INITIAL_JIFFIES;




