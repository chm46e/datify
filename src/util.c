#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#include "util.h"
#include "config.h"

extern void wlog(const char *msg)
{
	#if LOGGING
	FILE *log_file;
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);

	log_file = fopen("/tmp/dino.log", "a");
	fprintf(log_file, "[%d:%d] %s\n", tm.tm_hour, tm.tm_min, msg);

	fclose(log_file);
	#endif
}

extern void die(const char *err, int exit_code, int log_switch)
{
	if (log_switch)
		wlog(err);
	else
		printf("%s", err);

	exit(exit_code);
}

extern void *emalloc(int size)
{
	void *p;

	if(!(p = malloc(size)))
		die("Failed to allocate memory", 1, 1);
	return p;
}

extern void *ecalloc(int num, int size)
{
	void *p;

	if (!(p = calloc(num, size)))
		die("Failed to allocate memory", 1, 1);
	return p;
}

extern void efree(void *mem)
{
    free(mem);
}
