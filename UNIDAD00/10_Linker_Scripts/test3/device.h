#ifndef _DEVICE_H
#define _DEVICE_H

#define DEV_COUNT	2
#define lookup_dev(id)	dtable[id]
#define add_dev(dev)	dtable[(dev).id] = (dev)

typedef void (*_dev_write_cb) (unsigned char *, unsigned short);

struct device
{
	unsigned int	id;
	_dev_write_cb	write;
};

struct device dtable[DEV_COUNT];

int
register_device(struct device d);

void
dev_write(unsigned int devid, unsigned char *b, unsigned short len);

#endif /* _DEVICE_H */
