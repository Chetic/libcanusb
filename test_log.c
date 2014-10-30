#include <stdlib.h>
#include <stdio.h>
#include "log.h"

int main(int argc, char* argv[])
{
	const int x = 1337;

	log_init();
        log_write("Hello ");
        log_write("%d", 1337);
	log_close();

	return 0;
}
