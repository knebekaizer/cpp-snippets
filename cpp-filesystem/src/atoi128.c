/**
 * https://stackoverflow.com/a/45609367
 * License: Creative Common CC BY-SA 3.0
 */

#include <stdio.h>
#include <inttypes.h>

__int128 atoi128(const char *s)
{
    while (*s == ' ' || *s == '\t' || *s == '\n' || *s == '+') ++s;
    int sign = 1;
    if (*s == '-')
    {
        ++s;
        sign = -1;
    }
    size_t digits = 0;
    while (s[digits] >= '0' && s[digits] <= '9') ++digits;
    char scratch[digits];
    for (size_t i = 0; i < digits; ++i) scratch[i] = s[i] - '0';
    size_t scanstart = 0;

    __int128 result = 0;
    __int128 mask = 1;
    while (scanstart < digits)
    {
        if (scratch[digits-1] & 1) result |= mask;
        mask <<= 1;
        for (size_t i = digits-1; i > scanstart; --i)
        {
            scratch[i] >>= 1;
            if (scratch[i-1] & 1) scratch[i] |= 8;
        }
        scratch[scanstart] >>= 1;
        while (scanstart < digits && !scratch[scanstart]) ++scanstart;
        for (size_t i = scanstart; i < digits; ++i)
        {
            if (scratch[i] > 7) scratch[i] -= 3;
        }
    }

    return result * sign;
}


//int main(int argc, char **argv)
//{
//    if (argc > 1)
//    {
//        __int128 x = atoi128(argv[1]);
//        printf("%" PRIi64 "\n", (int64_t)x); // just for demo with smaller numbers
//    }
//}
