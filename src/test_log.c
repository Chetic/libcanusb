#include <stdio.h>
#include "log.h"

int main(int argc, char* argv[])
{
	const int x = 1337;
	char buf[1024];
	size_t nread;
	int success = 1;
	FILE* file;

	log_init();
	log_write("%d", 1337);
	log_close();

	file = fopen("log.txt", "r");
	if (file)
	{
		if (nread = fread(buf, 1, sizeof buf, file))
		{
			int i;
			char* correctStr = "1337";

			for (i = 0; i < nread; i++)
			{
				printf("%02x ", buf[i]);
				if (buf[i] != correctStr[i])
				{
					success = 0;
				}
			}
			printf("\n");
		}
		fclose(file);
	}

	if (success)
		printf("Success!\n");
	else
		printf("Test failed.\n");

	return (success == 1);
}
