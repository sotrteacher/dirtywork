#include "device.h"

int
register_device(struct device d)
{
	add_dev(d);
	return 0;	
}

void
dev_write(unsigned int devid, unsigned char *b, unsigned short len)
{
	lookup_dev(devid).write(b, len);
}

