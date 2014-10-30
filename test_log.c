#include <stdlib.h>
#include <stdio.h>
#include "log.h"

int main(int argc, char* argv[])
{
	const int x = 1337;
	char buf[1024];
	size_t nread;
	int success = 0;
	FILE* file;

	log_init();
        log_write("%d", 1337);
	log_close();

	file = fopen("log.txt", "r");
	if (file)
	{
		if (nread = fread(buf, 1, sizeof buf, file) > 0)
		{
			printf("Data read: %s\n", buf);
			if (strcmp("1337", buf))
			{
				success = 1;
			}
		}
	    	fclose(file);
	}
	
	if (success)
		printf("Success!");
	else
		printf("Test failed.");

	return (success == 1);
}
