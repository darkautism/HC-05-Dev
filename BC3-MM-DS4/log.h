#ifndef __LOG__H
#define __LOG__H

/*#define DEBUG_ENABLED*/

#ifdef DEBUG_ENABLED
#define LOG_DEBUG(x) \
    do               \
    {                \
        printf x;    \
    } while (0)
#else
#define LOG_DEBUG(x)
#endif

#endif
