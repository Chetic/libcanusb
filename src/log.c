#include <stdio.h>
#include <stdarg.h>
#include "log.h"

static FILE *f;

int log_init(void)
{
    f = fopen("log.txt", "w");

    if (f == NULL)
    {
        printf("Error opening file!\n");
        return -1;
    }
    return 0;
}

void log_close(void)
{
    fclose(f);
}

void log_write(const char* format, ...)
{
    va_list argptr;
    va_start(argptr, format);
    vfprintf(f, format, argptr);
    va_end(argptr);
}
