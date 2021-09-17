/* Skelix by Xiaoming Mo (skelixos@gmail.com)
 * Licence: GPLv2 */
#include <libcc.h>

/* result is currect, even when both area overlap */
void 
bcopy(const void *src, void *dest, unsigned int n) {
	const char *s = (const char *)src;
	char *d = (char *)dest;
	if (s <= d)
		for (; n>0; --n)
			d[n-1] = s[n-1];
	else
		for (; n>0; --n)
			*d++ = *s++;
}

void
bzero(void *dest, unsigned int n) {
	memset(dest, 0, n);
}

void *
memcpy(void *dest, const void *src, unsigned int n) {
	bcopy(src, dest, n);
	return dest;
}

void *
memset(void *dest, int c, unsigned int n) {
	char *d = (char *)dest;
	for (; n>0; --n)
		*d++ = (char)c;
	return dest;
}

int
memcmp(const void *s1, const void *s2, unsigned int n) {
	const char *s3 = (const char *)s1;
	const char *s4 = (const char *)s2;
	for (; n>0; --n) {
		if (*s3 > *s4)
			return 1;
		else if (*s3 < *s4)
			return -1;
		++s3;
		++s4;
	}
	return 0;
}

int
strcmp(const char *s1, const char *s2) {
	while (*s1 && *s2) {
		int r = *s1++ - *s2++;
		if (r)
			return r;
	}
	return (*s1)?1:-1;
}

char *
strcpy(char *dest, const char *src) {
	char *p = dest;
	while ( (*dest++ = *src++))
		;
	*dest = 0;
	return p;
}

unsigned int
strlen(const char *s) {
	unsigned int n = 0;
	while (*s++)
		++n;
	return n;
}
