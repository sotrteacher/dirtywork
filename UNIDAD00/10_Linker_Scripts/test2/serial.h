#ifndef _SERIAL_H
#define _SERIAL_H

#include "io.h"

/******************************************************************************
 * I/O ports
 ******************************************************************************/

/*
 * All the I/O ports are calculated relative to the data port.
 * This is because all serial ports (COM1, COM2, COM3 and COM4)
 * have their ports in the same order, but they start at
 * different values.
 */
#define SERIAL_COM1_BASE		0x3F8

#define SERIAL_DATA_PORT(base)		(base)
#define SERIAL_FIFO_CMD_PORT(base)	(base + 2)
#define SERIAL_LINE_CMD_PORT(base)	(base + 3)
#define SERIAL_MODEM_CMD_PORT(base)	(base + 4)
#define SERIAL_LINE_STATUS_PORT(base)	(base + 5)

/*
 * Register map for a given COM port:
 *
 *	+========+======================================================+
 *	| Offset | Description						|
 *	+========+======================================================+
 *	| +0     | Data register. If DLAB is disabled, reading from     |
 *	|        | this register is reading from the RECEIVE buffer and |
 *	|        | writing to it is the same as writing to the TRANSMIT |
 *	|        | buffer. If DLAB is enabled then this will hold the   |
 *	|        | low byte of the baud rate divisor value.             |
 *	+--------+------------------------------------------------------+
 *	| +1     | Interrupt enabling register. If DLAB is enabled,     |
 *	|        | this register will hold the high byte of the baud    |
 *	|        | rate divisor value.                                  |
 *	+--------+------------------------------------------------------+
 *	| +2     | Interrupt identification and buffer / FIFO config.   |
 *	+--------+------------------------------------------------------+
 *	| +3     | Line control register. Configures the line protocol. |
 *	+--------+------------------------------------------------------+
 *	| +4     | Modem control register.                              |
 *	+--------+------------------------------------------------------+
 *	| +5     | Line status register.                                |
 *	+--------+------------------------------------------------------+
 *	| +6     | Modem status register.                               |
 *	+--------+------------------------------------------------------+
 *	| +7     | Scratch register.                                    |
 *	+========+======================================================+
 */

/******************************************************************************
 * I/O port commands
 ******************************************************************************/

#define SERIAL_LINE_ENABLE_DLAB	0x80

struct com_port
{
	unsigned short	com;
	unsigned short	divisor;
};

void
serial_init(void);

void
serial_cfg_baudrate(unsigned short com, unsigned short divisor);
void
serial_cfg_line(unsigned short com);
void
serial_cfg_buf(unsigned short com);
void
serial_cfg_modem(unsigned short com);
void
serial_cfg_port(const struct com_port *p);

unsigned char
serial_is_tx_fifo_empty(unsigned short com);

void
serial_write(unsigned char *b, unsigned short len);

#endif /* _SERIAL_H */
