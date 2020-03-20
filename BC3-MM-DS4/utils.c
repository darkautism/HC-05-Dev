#include <csrtypes.h>
#include <string.h>
#include "utils.h"

unsigned char *tmp_ucp;
uint8 tmp_u8;

uint32 hexadecimalToDecimal(char hexVal[])
{
    int len = strlen(hexVal);
    uint32 base = 1;
    uint32 dec_val = 0;
    int i;

    for (i=len-1; i>=0; i--)
    {
        if (hexVal[i]>='0' && hexVal[i]<='9')
        {
            dec_val += (hexVal[i] - 48)*base;
            base = base * 16;
        }
        else if (hexVal[i]>='A' && hexVal[i]<='F')
        {
            dec_val += (hexVal[i] - 55)*base;
            base = base*16;
        }
    }

    return dec_val;
}

#ifndef BLUESTACK_VERSION_MAJOR
char *strstr(const char *s1, const char *s2)
{
    const char *p = s1;
    const size_t len = strlen(s2);

    for (; (p = strchr(p, *s2)) != 0; p++)
    {
        if (strncmp(p, s2, len) == 0)
            return (char *)p;
    }
    return (0);
}
#endif
