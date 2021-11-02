/* Skelix by Xiaoming Mo (skelixos@gmail.com)
 * Licence: GPLv2 */
#include <scr.h>
#include <asm.h>
#include <kprintf.h>

#define args_list char *

#define _arg_stack_size(type)	\
(((sizeof(type)-1)/sizeof(int)+1)*sizeof(int))

#define args_start(ap, fmt) do {	\
ap = (char *)((unsigned int)&fmt + _arg_stack_size(&fmt));	\
} while (0)

#define args_end(ap)

#define args_next(ap, type) (((type *)(ap+=_arg_stack_size(type)))[-1])

static char buf[1024] = {-1};
static int ptr = -1;

/* valid base: 2, 8, 10 */
static void
parse_num(unsigned int value, unsigned int base) {
	unsigned int n = value / base;
	int r = value % base;
	if (r < 0) {
		r += base;
		--n;
	}
	if (value >= base)
		parse_num(n, base);
	buf[ptr++] = "0123456789"[r];
}

static void
parse_hex(unsigned int value) {
	int i = 8;
	while (i-- > 0) {
		buf[ptr++] = "0123456789abcdef"[(value>>(i*4))&0xf];
	}
}

/* %s, %c, %x, %d, %% */
void
kprintf(enum KP_LEVEL kl, const char *fmt, ...) {
	int i = 0;
	char *s;
	/* must be the same size as enum KP_LEVEL */
	struct KPC_STRUCT {
		COLOUR fg;
		COLOUR bg;
	} KPL[] = {
		{BRIGHT_WHITE, BLACK},
		{YELLOW, RED},
	};

	args_list args;
	args_start(args, fmt);

	ptr = 0;
//goto out_kprintf;	
//#if 1 //2021.10.20 
//#if 0 //2021.10.20
//	while(!0){}
//#endif
	for (; fmt[i]; ++i) {
		if ((fmt[i]!='%') && (fmt[i]!='\\')) {
			buf[ptr++] = fmt[i];
			continue;
		} else if (fmt[i] == '\\') {
			/* \a \b \t \n \v \f \r \\ */
			switch (fmt[++i]) {
			case 'a': buf[ptr++] = '\a'; break;
			case 'b': buf[ptr++] = '\b'; break;
			case 't': buf[ptr++] = '\t'; break;
			case 'n': buf[ptr++] = '\n'; break;
			case 'r': buf[ptr++] = '\r'; break;
			case '\\':buf[ptr++] = '\\'; break;
			}
			continue;
		}
		/* fmt[i] == '%' */
		switch (fmt[++i]) {
		case 's':
			s = (char *)args_next(args, char *);
			while (*s)
				buf[ptr++] = *s++;
			break;
		case 'c':
			/* why is int?? */
			buf[ptr++] = (char)args_next(args, int);
			break;
		case 'x':
			parse_hex((unsigned long)args_next(args, unsigned long));
			break;
		case 'd':
			parse_num((unsigned long)args_next(args, unsigned long), 10);
			break;
		case '%':
			buf[ptr++] = '%';
			break;
		default:
			buf[ptr++] = fmt[i];
			break;
		}
	}
	buf[ptr] = '\0';
	args_end(args);
	for (i=0; i<ptr; ++i)
		print_c(buf[i], KPL[kl].fg, KPL[kl].bg);
//out_kprintf:
//    return;
}/*end kprintf()*/
