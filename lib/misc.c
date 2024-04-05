#include "misc.h"

u16 str_len(char* data)
{
    char* start = data;
    while (*data != 0) {
        data++;
    }
    return (u64)data - (u64)start;
}

str make_str(char* data, u16 len)
{
    if (len == 0) len = str_len(data);
    return (str) {
        .data = data, .len = len
    };
}