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
    log_write("%c", 0x37);
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
    fprintf(f, format, argptr);
    va_end(argptr);
}
