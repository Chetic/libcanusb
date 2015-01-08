#include <stdio.h>
#include <stdarg.h>
#include "log.h"

static FILE *f;
static char buf[LOG_BUFFER_SIZE];

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

	//Store to buffer and print to both logfile and stdout:
	vsnprintf (buf, LOG_BUFFER_SIZE, format, argptr);
	fputs(buf, f);
	fputs(buf, stdout);
	fflush(stdout);

	va_end(argptr);
}
