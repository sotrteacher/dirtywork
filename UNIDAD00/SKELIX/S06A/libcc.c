/* Skelix by Xiaoming Mo (skelixos@gmail.com)
 * Licence: GPLv2 */
#include <libcc.h>
#if 1 //LMC 2021.09.19
#include <kprintf.h>
void copy_data_byte(const char *s,char *dest);
#endif

/* result is currect, even when both area overlap */
void 
bcopy(const void *src, void *dest, unsigned int n) {
	const char *s = (const char *)src;
	char *d = (char *)dest;
static int x=0;
static int y=0;

	kprintf(KPL_DUMP,"bcopy(): dest=%x,\tsrc=%x\tx=%d\ty=%d\tn=%d\n",dest,src,x,y,n);
	if (s <= d)
		for (; n>0; --n){
		#if 1 //LMC 2021.09.19
			d[n-1] = s[n-1];
		#else
			copy_data_byte(s+n-1,d+n-1);
//if(x>=296){//Con 295 No hay INTERRUPT 655040, Con 296 Si hay INTERRUPT 655040
//for(;;);   //Significa hay INTERRUPT 655040 cuando x es >= 296
//}
		#endif
x++;
		}
	else
		for (; n>0; --n){
		#if 1 //LMC 2021.09.19 
			*d++ = *s++;
		#else
//if(y==2){
//for(;;);
//}
			copy_data_byte(s++,d++);
		#endif
y++;
		}
}

void
bzero(void *dest, unsigned int n) {
	memset(dest, 0, n);
}

void *
memcpy(void *dest, const void *src, unsigned int n) {
#if 1 //LMC 2021.09.19
	kprintf(KPL_DUMP,"memcpy(): dest=%x,\tsrc=%x\n",dest,src);
#endif
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
