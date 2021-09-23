#ifndef _UTILS_H
#define _UTILS_H

enum PRINTK_LVL
{
	KERN_INFO = 0,
	KERN_WARN,
	KERN_ERROR
};

#if 0
void
printk(unsigned char lvl, unsigned const char *msg, ...);
#endif

#endif /* _UTILS_H */
