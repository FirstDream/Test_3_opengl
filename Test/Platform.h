
#ifndef __PLATFORM__H
#define __PLATFORM__H

static char *strndup(const char *s1, size_t n)
{
	char *copy = (char*)malloc(n + 1);
	memcpy(copy, s1, n);
	copy[n] = 0;
	return copy;
};

#endif